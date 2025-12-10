namespace CodexiumMagnus.Services
{
    /// <summary>
    /// Bridge interface for communicating with the embedded CEF window.
    /// </summary>
    public interface ICefBridge
    {
        void Send(string json);
    }
}