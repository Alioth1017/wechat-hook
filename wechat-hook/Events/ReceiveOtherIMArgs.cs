namespace wechat_hook.Events
{
    public class ReceiveOtherIMArgs
    {
        public int Type { get; set; }

        public string FromWxid { get; set; }

        public string SendWxid { get; set; }

        public string Content { get; set; }

        public string Other { get; set; }

    }
}
