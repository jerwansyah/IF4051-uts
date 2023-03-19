<template>
  <div class="w-[320px]">
    <h1 class="text-2xl mb-2">Smart System</h1>
    <h3 class="text-m mb-8">13519116 Jeane Mikha Erwansyah</h3>
    <h3
      class="rounded-xl border-2 border-slate-200 p-4 mb-4 select-none transition-all hover:cursor-pointer"
      :class="{ 'bg-slate-200': isLampOn }"
      @click="handleLampToggle"
    >
      Lamp<span v-if="isLampOn"> - {{ lampTimer }}</span>
    </h3>
    <h3
      class="rounded-xl border-2 border-slate-200 p-4 mb-8 select-none transition-all hover:cursor-pointer"
      :class="{ 'bg-slate-200': isACOn }"
      @click="handleACToggle"
    >
      AC<span v-if="isACOn"> - {{ ACTimer }}</span>
    </h3>

    <h2 class="mb-4">Timer (to toggle)</h2>
    <div class="flex gap-2">
      <h3>Lamp</h3>
      <input type="text" class="w-16 text-right" />
      <p>minutes</p>
      <button
        class="text-sm rounded-lg px-4 py-2 bg-slate-200 hover:bg-slate-300 active:bg-slate-400"
      >
        Set
      </button>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      isLampOn: false,
      isACOn: false,

      lampTimer: 0,
      ACTimer: 0,
    };
  },
  mounted() {
    this.socket = new WebSocket("ws://127.0.0.1:3001");

    this.socket.addEventListener("open", (event) => {
      console.log("WebSocket connection established");
    });

    this.socket.addEventListener("error", (event) => {
      console.error("WebSocket connection error", event);
    });

    this.socket.addEventListener("close", (event) => {
      console.log("WebSocket connection closed", event);
    });

    this.socket.addEventListener("message", (event) => {
      console.log("Received message from server", event.data);
    });
  },
  methods: {
    handleLampToggle() {
      this.isLampOn = !this.isLampOn;
    },
    handleACToggle() {
      this.isACOn = !this.isACOn;
    },
  },
};
</script>
