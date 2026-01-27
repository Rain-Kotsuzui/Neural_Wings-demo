<script setup>
import { computed, onBeforeUnmount, onMounted, ref } from "vue";

// Global state for C++ communication
window.vueAppState = window.vueAppState || {};
window.vueAppState.vueAppReady = false;
window.vueAppState.settingsSaveRequested = false;

const resolutions = [
  { label: "1280x720", value: "1280x720" },
  { label: "1600x900", value: "1600x900" },
  { label: "1920x1080", value: "1920x1080" },
  { label: "2560x1440", value: "2560x1440" },
];

const route = ref(window.location.hash || "#/start");
const fullscreen = ref(null);
const selectedResolution = ref("");
const isResolutionOpen = ref(false);
const targetFPS = ref(null);

const isStart = computed(() => route.value.startsWith("#/start"));
const isMenu = computed(() => route.value.startsWith("#/menu"));
const isOptions = computed(() => route.value.startsWith("#/options"));

function syncRoute() {
  route.value = window.location.hash || "#/start";
  window.vueAppState.currentRoute = route.value;
}

function syncAppStateToWindow() {
  if (fullscreen.value !== null) {
    window.vueAppState.fullscreen = fullscreen.value;
  }
  if (selectedResolution.value) {
    window.vueAppState.resolution = selectedResolution.value;
  }
  if (targetFPS.value !== null) {
    window.vueAppState.targetFPS = targetFPS.value;
  }
}

function applySettings(settings) {
  const state = settings || {};

  if (typeof state.fullscreen === "boolean") {
    fullscreen.value = state.fullscreen;
  }

  if (typeof state.resolution === "string" && state.resolution) {
    const exists = resolutions.some((r) => r.value === state.resolution);
    if (exists) {
      selectedResolution.value = state.resolution;
    }
  }

  if (state.targetFPS !== undefined && state.targetFPS !== null) {
    const fps = Number(state.targetFPS);
    if (!Number.isNaN(fps)) {
      targetFPS.value = fps;
    }
  }

  syncAppStateToWindow();
}

function toggleFullscreen() {
  if (fullscreen.value === null) {
    return;
  }
  fullscreen.value = !fullscreen.value;
}

function toggleResolution() {
  isResolutionOpen.value = !isResolutionOpen.value;
}

function chooseResolution(value) {
  selectedResolution.value = value;
  isResolutionOpen.value = false;
}

function changeFPS(value) {
  targetFPS.value = value;
}

function saveSettings() {
  if (
    fullscreen.value === null ||
    !selectedResolution.value ||
    targetFPS.value === null
  ) {
    return;
  }

  window.vueAppState.fullscreen = fullscreen.value;
  window.vueAppState.resolution = selectedResolution.value;
  window.vueAppState.targetFPS = targetFPS.value;
  window.vueAppState.settingsSaveRequested = true;

  console.log("Settings save requested:", {
    fullscreen: fullscreen.value,
    resolution: selectedResolution.value,
    fps: targetFPS.value,
  });
}

function handleDocumentClick() {
  isResolutionOpen.value = false;
}

onMounted(() => {
  window.addEventListener("hashchange", syncRoute);
  document.addEventListener("click", handleDocumentClick);

  syncRoute();

  // Replace the early stub from main.js with the real handler.
  window.__applyEngineSettings = applySettings;

  // Apply settings that may have arrived before mount.
  if (window.__pendingEngineSettings) {
    applySettings(window.__pendingEngineSettings);
    window.__pendingEngineSettings = undefined;
  }

  window.vueAppState.vueAppReady = true;
});

onBeforeUnmount(() => {
  window.removeEventListener("hashchange", syncRoute);
  document.removeEventListener("click", handleDocumentClick);
});
</script>

<template>
  <section v-if="isStart" class="splash-only">
    <h1 class="splash-title">Neural Wings</h1>
  </section>

  <div v-else class="root">
    <header class="topbar">
      <div class="brand">Neural Wings</div>
    </header>

    <main class="content">
      <section v-if="isMenu" class="card">
        <h1>主菜单</h1>
        <p>欢迎来到 Neural Wings</p>
        <div class="actions">
          <a class="btn" href="#/gameplay">开始游戏</a>
          <a class="btn" href="#/options">游戏设置</a>
        </div>
      </section>

      <section v-else-if="isOptions" class="card">
        <h1>游戏设置</h1>
        <div class="form">
          <div class="row">
            <span>全屏</span>
            <button
              class="chip"
              @click="toggleFullscreen"
              :class="{ active: fullscreen === true }"
            >
              {{ fullscreen === null ? "未设置" : fullscreen ? "开启" : "关闭" }}
            </button>
          </div>

          <div class="row">
            <span>分辨率</span>
            <div class="dropdown" @click.stop>
              <button class="chip dropdown-toggle" @click="toggleResolution">
                {{ selectedResolution || "请选择" }}
                <span class="caret"></span>
              </button>
              <div v-if="isResolutionOpen" class="dropdown-menu">
                <button
                  v-for="res in resolutions"
                  :key="res.value"
                  class="dropdown-item"
                  @click="chooseResolution(res.value)"
                >
                  {{ res.label }}
                </button>
              </div>
            </div>
          </div>

          <div class="row">
            <span>帧率上限</span>
            <div class="fps-control">
              <input
                type="range"
                v-model.number="targetFPS"
                @change="changeFPS(targetFPS)"
                min="30"
                max="240"
                step="10"
                :disabled="targetFPS === null"
              />
              <span class="chip">{{ targetFPS === null ? "--" : targetFPS }} FPS</span>
            </div>
          </div>
        </div>

        <div class="actions">
          <a class="btn" href="#/menu">返回菜单</a>
          <button class="btn ghost" @click="saveSettings">保存设置</button>
        </div>
      </section>

      <section v-else class="card">
        <h1>未找到页面</h1>
        <a class="btn" href="#/start">返回启动</a>
      </section>
    </main>
  </div>
</template>
