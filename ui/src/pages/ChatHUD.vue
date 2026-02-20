<script setup>
import { ref, nextTick, onMounted, onBeforeUnmount } from "vue";

const messages = ref([]);
const inputText = ref("");
const isActive = ref(false);
const inputRef = ref(null);
const messagesEndRef = ref(null);
const route = ref(window.location.hash || "");
const pendingSendQueue = [];
let sendFlushTimer = null;

function syncRoute() {
  route.value = window.location.hash || "";
  if (!isVisibleRoute() && isActive.value) {
    deactivate();
  }
}

function isVisibleRoute() {
  const r = route.value;
  return (
    r.startsWith("#/menu") ||
    r.startsWith("#/options") ||
    r.startsWith("#/gameplay")
  );
}

function isGameplayRoute() {
  return route.value.startsWith("#/gameplay");
}

function shouldHandleLocalHotkeys() {
  return route.value.startsWith("#/menu") || route.value.startsWith("#/options");
}

function scrollToBottom() {
  nextTick(() => {
    if (messagesEndRef.value) {
      messagesEndRef.value.scrollIntoView({ behavior: "auto" });
    }
  });
}

function pushMessage(msg) {
  messages.value.push(msg);
  if (messages.value.length > 300) {
    messages.value.shift();
  }
  scrollToBottom();
}

function pushBatch(batch) {
  if (!Array.isArray(batch) || batch.length === 0) {
    return;
  }
  for (const msg of batch) {
    messages.value.push(msg);
  }
  if (messages.value.length > 300) {
    messages.value.splice(0, messages.value.length - 300);
  }
  scrollToBottom();
}

function activate() {
  isActive.value = true;
  window.vueAppState = window.vueAppState || {};
  window.vueAppState.chatActive = true;
  nextTick(() => {
    if (inputRef.value) {
      inputRef.value.focus();
    }
  });
}

function deactivate() {
  isActive.value = false;
  inputText.value = "";
  window.vueAppState = window.vueAppState || {};
  window.vueAppState.chatActive = false;
}

function clearInput() {
  inputText.value = "";
  syncInputToAppState();
}

function syncInputToAppState() {
  window.vueAppState = window.vueAppState || {};
  window.vueAppState.chatInputText = inputText.value.trim();
}

function onInput() {
  // Keep vueAppState in sync whenever the user types
  syncInputToAppState();
}

function flushPendingSends() {
  if (pendingSendQueue.length === 0) {
    return;
  }
  window.vueAppState = window.vueAppState || {};
  if (window.vueAppState.chatSendRequested === true) {
    return;
  }
  const nextText = pendingSendQueue.shift();
  window.vueAppState.chatSendText = nextText;
  window.vueAppState.chatSendRequested = true;
}

function isEnterKey(e) {
  return (
    e.key === "Enter" ||
    e.code === "Enter" ||
    e.code === "NumpadEnter" ||
    e.keyCode === 13
  );
}

function isEscapeKey(e) {
  return (
    e.key === "Escape" ||
    e.key === "Esc" ||
    e.code === "Escape" ||
    e.keyCode === 27
  );
}

function onInputKeydown(e) {
  const enter = isEnterKey(e);
  const escape = isEscapeKey(e);
  if (!enter && !escape) {
    return;
  }

  e.preventDefault();
  e.stopPropagation();

  if (shouldHandleLocalHotkeys()) {
    if (enter) {
      submitInput();
      return;
    }
    onEscapePressed();
  }
}

function submitInput() {
  const text = inputText.value.trim();
  if (!text) {
    return;
  }

  if (pendingSendQueue.length < 128) {
    pendingSendQueue.push(text);
  }
  clearInput();
  flushPendingSends();
}

function onEscapePressed() {
  deactivate();
}

function onGlobalKeydown(e) {
  if (!isVisibleRoute() || !shouldHandleLocalHotkeys()) {
    return;
  }

  if (isEnterKey(e)) {
    e.preventDefault();
    e.stopPropagation();

    if (!isActive.value) {
      activate();
      return;
    }
    submitInput();
    return;
  }

  if (isEscapeKey(e) && isActive.value) {
    e.preventDefault();
    e.stopPropagation();
    onEscapePressed();
  }
}

function getTypeClass(type) {
  switch (type) {
    case "system":
      return "msg-system";
    case "whisper":
      return "msg-whisper";
    default:
      return "msg-public";
  }
}

function getTypeLabel(type) {
  switch (type) {
    case "system":
      return "[SYSTEM]";
    case "whisper":
      return "[WHISPER]";
    default:
      return "";
  }
}

onMounted(() => {
  // Register global hooks for C++ to call
  window.__NW_CHAT_PUSH__ = pushMessage;
  window.__NW_CHAT_PUSH_BATCH__ = pushBatch;
  window.__NW_CHAT_ACTIVATE__ = activate;
  window.__NW_CHAT_DEACTIVATE__ = deactivate;
  window.__NW_CHAT_CLEAR_INPUT__ = clearInput;

  // Init vueAppState
  window.vueAppState = window.vueAppState || {};
  window.vueAppState.chatInputText = "";
  window.vueAppState.chatSendText = "";
  window.vueAppState.chatSendRequested = false;
  sendFlushTimer = window.setInterval(flushPendingSends, 16);
  syncRoute();
  window.addEventListener("hashchange", syncRoute);
  window.addEventListener("keydown", onGlobalKeydown);
});

onBeforeUnmount(() => {
  if (sendFlushTimer !== null) {
    window.clearInterval(sendFlushTimer);
    sendFlushTimer = null;
  }
  delete window.__NW_CHAT_PUSH__;
  delete window.__NW_CHAT_PUSH_BATCH__;
  delete window.__NW_CHAT_ACTIVATE__;
  delete window.__NW_CHAT_DEACTIVATE__;
  delete window.__NW_CHAT_CLEAR_INPUT__;
  window.removeEventListener("hashchange", syncRoute);
  window.removeEventListener("keydown", onGlobalKeydown);
});
</script>

<template>
  <div v-if="isVisibleRoute()" class="chat-hud" :class="{ active: isActive, gameplay: isGameplayRoute() }">
    <!-- Message history (always visible, semi-transparent when inactive) -->
    <div v-if="isActive || isGameplayRoute()" class="chat-messages" :class="{ faded: !isActive }">
      <div
        v-for="(msg, idx) in messages"
        :key="idx"
        class="chat-line"
        :class="getTypeClass(msg.type)"
      >
        <span v-if="getTypeLabel(msg.type)" class="chat-type-label">{{
          getTypeLabel(msg.type)
        }}</span>
        <span v-if="msg.senderName && msg.type !== 'system'" class="chat-sender"
          >{{ msg.senderName }}:</span
        >
        <span class="chat-text">{{ msg.text }}</span>
      </div>
      <div ref="messagesEndRef"></div>
    </div>

    <!-- Input bar (only when active) -->
    <div v-if="isActive" class="chat-input-bar">
      <input
        ref="inputRef"
        v-model="inputText"
        type="text"
        class="chat-input"
        placeholder="Type a message... (/w <id> for whisper)"
        maxlength="256"
        @keydown="onInputKeydown"
        @input="onInput"
      />
    </div>
  </div>
</template>

<style scoped>
.chat-hud {
  position: fixed;
  bottom: 48px;
  left: 20px;
  width: 640px;
  max-width: 68vw;
  z-index: 9999;
  pointer-events: none;
  font-family: "Consolas", "Courier New", monospace;
  font-size: 16px;
}

.chat-hud.active {
  pointer-events: auto;
}

.chat-messages {
  max-height: 320px;
  overflow-y: auto;
  padding: 10px 12px;
  display: flex;
  flex-direction: column;
  gap: 6px;
  /* Hide scrollbar for cleaner look */
  scrollbar-width: thin;
  scrollbar-color: rgba(255, 255, 255, 0.15) transparent;
}

.chat-messages.faded {
  opacity: 0.5;
}

.chat-line {
  padding: 7px 10px;
  border-radius: 6px;
  background: rgba(0, 0, 0, 0.55);
  overflow-wrap: break-word;
  line-height: 1.5;
}

.chat-hud.gameplay .chat-line {
  background: rgba(0, 0, 0, 0.18);
}

.msg-system {
  color: #ffd740;
}

.msg-public {
  color: #dfe6dd;
}

.msg-whisper {
  color: #b388ff;
}

.chat-type-label {
  font-weight: bold;
  margin-right: 8px;
  font-size: 13px;
}

.chat-sender {
  font-weight: bold;
  margin-right: 8px;
  color: #8aff7f;
}

.msg-whisper .chat-sender {
  color: #ce93d8;
}

.msg-system .chat-sender {
  color: #ffd740;
}

.chat-text {
  /* keep normal weight */
}

.chat-input-bar {
  margin-top: 8px;
}

.chat-input {
  width: 100%;
  padding: 12px 14px;
  background: rgba(0, 0, 0, 0.75);
  border: 1px solid rgba(138, 255, 127, 0.4);
  border-radius: 8px;
  color: #dfe6dd;
  font-family: inherit;
  font-size: 16px;
  outline: none;
}

.chat-hud.gameplay .chat-input {
  background: rgba(0, 0, 0, 0.28);
}

.chat-input:focus {
  border-color: rgba(138, 255, 127, 0.8);
  box-shadow: 0 0 6px rgba(138, 255, 127, 0.2);
}

.chat-input::placeholder {
  color: rgba(255, 255, 255, 0.3);
}

@media (max-width: 960px) {
  .chat-hud {
    left: 12px;
    right: 12px;
    bottom: 16px;
    width: auto;
    max-width: none;
    font-size: 14px;
  }

  .chat-messages {
    max-height: 220px;
    padding: 8px 10px;
    gap: 4px;
  }

  .chat-line {
    padding: 6px 8px;
  }

  .chat-input {
    padding: 10px 12px;
    font-size: 14px;
  }
}
</style>
