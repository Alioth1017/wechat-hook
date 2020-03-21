using Newtonsoft.Json.Linq;

namespace wechat_hook
{
    public class SocketRespons
    {
        public int Code { get; set; }

        public int Error { get; set; }

        public string Message { get; set; }

        public int Pid { get; set; }

        public string Callback { get; set; }

        public JToken Data { get; set; } 
    }
}
