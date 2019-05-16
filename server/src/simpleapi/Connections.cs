using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Net.WebSockets;



namespace simpleapi 
{
    public static class Connections
    {
        private static List<WebSocket> connections = new List<WebSocket>(); 

        public static void Add(WebSocket webSocket)
        {
            connections.Add(webSocket);
        }

        public async static Task SendToAll(string message) 
        {
            ArraySegment<byte> buffer = new ArraySegment<byte>(new byte[1024]);
            buffer = new ArraySegment<byte>(Encoding.UTF8.GetBytes(message));

            foreach (var socket in connections.Where((socket) => socket.State == WebSocketState.Open))
            {
                await socket.SendAsync(buffer, WebSocketMessageType.Text, true, CancellationToken.None);
            };
        }
    }
}