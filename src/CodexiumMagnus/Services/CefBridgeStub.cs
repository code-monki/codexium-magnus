// filepath: src/CodexiumMagnus/Services/CefBridgeStub.cs
using System;
using System.Diagnostics;

namespace CodexiumMagnus.Services
{
    /// <summary>
    /// Temporary CEF bridge that just logs outbound messages.
    /// Writes to both Debug and Console so it works in CLI runs.
    /// </summary>
    public sealed class CefBridgeStub : ICefBridge
    {
        public void Send(string json)
        {
            var line = $"[CEF STUB] {json}";

            // shows up in IDE / attached debugger
            Debug.WriteLine(line);

            // shows up in `dotnet run` from terminal
            Console.WriteLine(line);
        }
    }
}