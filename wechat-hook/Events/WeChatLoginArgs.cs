using Fleck;
using System;

namespace wechat_hook.Events
{
    public class WeChatLoginArgs : EventArgs
    {
        /// <summary>
        /// 登录的用户信息
        /// </summary>
        public User User { get; set; }

        /// <summary>
        /// 连接信息
        /// </summary>
        public IWebSocketConnection Connection { get; set; }

        /// <summary>
        /// 当前微信进程Id
        /// </summary>
        public int ProcessId { get; set; }
    }
}
