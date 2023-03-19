<template>
  <div class="w-[320px]">
    <h1 class="text-2xl mb-2">Smart System</h1>
    <h3 class="text-m mb-8">13519116 Jeane Mikha Erwansyah</h3>
    <h3
      class="rounded-xl border-2 border-slate-200 p-4 mb-4 select-none transition-all hover:cursor-pointer"
      :class="{ 'bg-slate-200': isLampOn }"
      @click="handleLampToggle"
    >
      Lamp<span v-if="isLampOn"> - {{ lampTime }}</span>
    </h3>
    <h3
      class="rounded-xl border-2 border-slate-200 p-4 mb-8 select-none transition-all hover:cursor-pointer"
      :class="{ 'bg-slate-200': isACOn }"
      @click="handleACToggle"
    >
      AC<span v-if="isACOn"> - {{ acTime }}</span>
    </h3>

    <h2 class="mb-4">Timer (to toggle)</h2>
    <div class="flex gap-2 mb-4">
      <h3 class="leading-none py-2">Lamp</h3>
      <div class="ml-auto flex">
        <input
          v-model="lampTimer"
          type="text"
          class="w-12 text-right bg-slate-100 rounded-md px-2"
        />
        <p class="leading-none py-2 ml-2">seconds</p>
        <button
          class="ml-10 text-sm rounded-lg px-8 py-2 bg-slate-200 hover:bg-slate-300 active:bg-slate-400"
          @click="handleLampTimer"
        >
          Set
        </button>
      </div>
    </div>
    <div class="flex gap-2 mb-12">
      <h3 class="leading-none py-2">AC</h3>
      <div class="ml-auto flex">
        <input
          v-model="acTimer"
          type="text"
          class="w-12 text-right bg-slate-100 rounded-md px-2"
        />
        <p class="leading-none py-2 ml-2">seconds</p>
        <button
          class="ml-10 text-sm rounded-lg px-8 py-2 bg-slate-200 hover:bg-slate-300 active:bg-slate-400"
          @click="handleACTimer"
        >
          Set
        </button>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      isLampOn: false,
      isACOn: false,

      lampTime: 0,
      acTime: 0,

      lampTimer: 5,
      acTimer: 10,
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
      this.handleStates(event.data);
    });
  },
  methods: {
    handleLampToggle() {
      this.isLampOn = !this.isLampOn;
      const data = {
        topic: "lamp/command",
        message: this.isLampOn ? "ON" : "OFF",
      };
      this.socket.send(JSON.stringify(data));
    },
    handleACToggle() {
      this.isACOn = !this.isACOn;
      const data = {
        topic: "ac/command",
        message: this.isACOn ? "ON" : "OFF",
      };
      this.socket.send(JSON.stringify(data));
    },
    handleStates(data) {
      const { topic, message } = JSON.parse(data);
      if (topic === "lamp/status") {
        this.isLampOn = message === "ON";
      } else if (topic === "ac/status") {
        this.isACOn = message === "ON";
      } else if (topic === "lamp/time") {
        this.lampTime = this.getTime(message);
      } else if (topic === "ac/time") {
        this.acTime = this.getTime(message);
      }
    },
    handleLampTimer() {
      const data = {
        topic: "lamp/timer",
        message: this.lampTimer,
      };
      this.socket.send(JSON.stringify(data));
    },
    handleACTimer() {
      const data = {
        topic: "ac/timer",
        message: this.acTimer,
      };
      this.socket.send(JSON.stringify(data));
    },
    sec2min(sec) {
      return Math.floor(sec / 60);
    },
    getTime(millis) {
      if (millis < 60000) {
        return Math.floor(millis / 1000) + " seconds up";
      }
      return this.sec2min(Math.floor(millis / 1000)) + " minutes up";
    },
  },
};
</script>
