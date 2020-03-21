using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;

namespace wechat_hook
{
    class Program
    {
        static WeChatSdk WeChat;
        static int pid;
        static List<Contact> Contacts = new List<Contact>();
        static void Main(string[] args)
        {
            WeChat = new WeChatSdk();
            WeChat.LogEvent += WeChat_LogEvent;
            WeChat.ReceiveContactEvent += WeChat_ReceiveContactEvent;
            WeChat.ReceiveOtherIMEvent += WeChat_ReceiveOtherIMEvent;
            WeChat.WeChatInitEvent += WeChat_WeChatInitEvent;
            WeChat.ConnetionCloseEvent += WeChat_ConnetionCloseEvent;
            OpenWeChat();
            while (Console.ReadKey().Key.ToString() != "C") { }
        }

        static void WeChat_LogEvent(object sender, string e)
        {
            ShowLog($"[日志]{e}");
        }

        static void WeChat_ReceiveContactEvent(object sender, Contact e)
        {
            if (e != null)
            {
                Contacts.Add(e);
                ShowLog($"[联系人]{Newtonsoft.Json.JsonConvert.SerializeObject(e)}");
            }
        }

        static void WeChat_ReceiveOtherIMEvent(object sender, wechat_hook.Events.ReceiveOtherIMArgs e)
        {
            ShowLog($"[消息]{e.Content}");
            ShowLog($"[消息]{Newtonsoft.Json.JsonConvert.SerializeObject(e)}");
        }

        static void WeChat_WeChatInitEvent(object sender, wechat_hook.Events.WeChatInitArgs e)
        {
            pid = e.PorcessId;
            ShowLog($"[日志]{e.Message}");
            while (!WeChat.CheckLoginState(e.PorcessId))
            {
                Thread.Sleep(1000);
            }
            var user = WeChat.GetLoginUser(e.PorcessId);
            if (user != null)
            {
                ShowLog($"[已登录]{Newtonsoft.Json.JsonConvert.SerializeObject(user)}");
            }
            Contacts.Clear();
            WeChat.GetContactList(pid);
        }

        static void WeChat_ConnetionCloseEvent(object sender, wechat_hook.Events.ConnetionCloseArgs e)
        {
            ShowLog($"[日志]websocket连接断开,请检查微信是否正常运行。");
        }
        static void OpenWeChat()
        {
            try
            {
                var ret = WeChat.OpenWechat();
                var list = Process.GetProcessesByName("WeChat");
                if (list.Length > 0)
                    ret = list[0].Id;
                if (ret == 0) return;
                if (!WeChat.IsInjected(ret))
                {
                    WeChat.InjectDll(pid);
                }
            }
            catch (Exception ex)
            {
                ShowLog($"[异常]{ex.Message}");
            }

        }
        static void ShowLog(string msg)
        {
            Console.WriteLine(msg);
        }
    }
}
