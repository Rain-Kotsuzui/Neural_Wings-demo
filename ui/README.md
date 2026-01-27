# Neural Wings UI (Vue + Vite)

This UI is now a proper Vue 3 + Vite project that still builds into the engine-friendly shape expected by the Ultralight runtime:

- `ui/dist/index.html`
- `ui/dist/assets/app.js`
- `ui/dist/assets/style.css`

The build config is in `ui/vite.config.js`.

## Commands

From `ui/`:

1. Install deps: `npm ci`
2. Dev server: `npm run dev`
3. Production build: `npm run build`
4. Preview build: `npm run preview`

## Engine integration contract

The engine communicates with the UI through globals on `window`:

- `window.vueAppState`
- `window.__applyEngineSettings(settings)`

These are defined in `ui/src/App.vue` and `ui/src/main.js`.
