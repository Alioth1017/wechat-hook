using Fleck;

namespace wechat_hook
{
    public class UserConnection : User
    {
        public IWebSocketConnection Connection { get; set; }

        public int ProcessId { get; set; }
    }
}
