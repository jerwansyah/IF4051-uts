const mqtt = require("mqtt");
const WebSocket = require("ws");

const MQTT_BROKER_URL = "mqtt://127.0.0.1:1883";
const MQTT_TOPICS = ["lamp/status", "ac/status", "lamp/time", "ac/time"];

// connect to the mqtt broker
const client = mqtt.connect(MQTT_BROKER_URL);

// handle mqtt connect event
client.on("connect", () => {
  console.log("Connected to MQTT broker");
  // subscribe to the mqtt topics
  client.subscribe(MQTT_TOPICS);
});

// handle mqtt message event
client.on("message", (topic, message) => {
  console.log(`Received MQTT message on topic ${topic}: ${message.toString()}`);
  // relay the message to connected clients
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify({ topic, message: message.toString() }));
    }
  });
});

// create websocket server
const port = 3001;
const wss = new WebSocket.Server({ port });

wss.on("connection", (ws) => {
  console.log("WebSocket connection opened.");

  ws.on("message", (message) => {
    console.log(`Received message from client: ${message}`);

    // publish the message to mqtt server
    try {
      const obj = JSON.parse(message);
      client.publish(obj.topic, obj.message);
    } catch {
      console.log("Invalid message format");
    }
  });

  ws.on("close", () => {
    console.log("WebSocket connection closed.");
  });
});
