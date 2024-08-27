<template>
    <h3>hello</h3>
    <input>
    <button id="sendMessage1">获取消息</button>
    <button id="sendMessage2">获取消息</button>
    <button id="sendMessage3">获取消息</button>
    <button id="sendMessage4">获取消息</button>
</template>

<script>
    const ws = new WebSocket("ws://localhost:9002");
    ws.onopen = function() {
        console.log("Connected to server");
    };

    ws.onmessage = function(event) {
        console.log(event.data);
    };

    ws.onclose = function() {
        console.log("Disconnected from server");
    };

    function sendNumber() {
        const input = document.getElementById("sendMessage").value;
    
    }
</script>