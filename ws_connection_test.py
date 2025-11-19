import asyncio
import websockets

async def client(name):
    uri = "ws://ПУБЛИЧНЫЙ IP СЕРВЕРА:8080"
    async with websockets.connect(uri) as websocket:
        print(f"{name} connected")
        while True:
            msg = await websocket.recv()
            print(f"{name} received: {msg}")

async def main():
    await asyncio.gather(
        client("Client1"),
        client("Client2"),
        client("Client3"),
    )

asyncio.run(main())