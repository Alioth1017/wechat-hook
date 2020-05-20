#include "stdafx.h"
#include "WSClient.h"
#include "Loger.h" 
#include "Command.h" 
#include "websocketpp/base64/base64.hpp"
#include "Init.h"
#include "Util.h" 
#include "EVString.h"
#include "Login.h"
#include "ContactList.h"
#include "ChatRecord.h"
#include "ChatRoom.h"
#include "Sql.h"
#include "SendMsg.h"
#include "UserInfo.h"

using namespace std;

client wsClient;
client::connection_ptr con;
const string wsurl = "ws://127.0.0.1:8181";

void InitWebSocket() {

Start:
	try
	{

		wsClient.set_access_channels(websocketpp::log::alevel::all);
		wsClient.clear_access_channels(websocketpp::log::alevel::frame_payload);

		wsClient.init_asio();
		wsClient.set_reuse_addr(true);

		wsClient.set_message_handler(bind(&RecvMessage, &wsClient, ::_1, ::_2));
		wsClient.set_close_handler(bind(&OnClose, &wsClient, ::_1));
		wsClient.set_open_handler(bind(&OnOpen, &wsClient, ::_1));

	Next:
		websocketpp::lib::error_code ec;
		con = wsClient.get_connection(wsurl, ec);
		if (ec) {
			Sleep(1000);//发生异常，休眠一秒后再次连接
			goto Next;
		}
		con->set_termination_handler(bind(OnTermination, &wsClient, ::_1));
		con = wsClient.connect(con);
		while (true)
		{
		Run:
			try
			{
				switch (con->get_state())
				{
				case 0:
					Sleep(1000);
					break;
				case 2:
				case 3:
					Sleep(1000);
					goto Next;
				default:
					break;
				}
				wsClient.run_one();
				Sleep(10);
			}
			catch (const std::exception& e)
			{
				string log = e.what();
				Sleep(1000);
				goto Run;
			}

		}
	}
	catch (websocketpp::exception const& e) {
		string log = e.what();
		Sleep(1000);
		goto Start;
	}
}

void StopWebSocket() {
	try
	{
		//WriteInfo("WebSocket Stop.");

		if (con != NULL && con->get_state() == websocketpp::session::state::value::open)
		{
			websocketpp::close::status::value cvValue = 0;
			std::string strReason = "";
			con->close(cvValue, strReason);
		}
	}
	catch (...)
	{

	}
}


void OnOpen(client* c, websocketpp::connection_hdl hdl) {
	try {
		InitWeChat();
	}
	catch (...) {
		neb::CJsonObject json;
		Send(Cmd_Init, json, "", 1, L"初始化失败。");
	}
}

void OnClose(client* c, websocketpp::connection_hdl hdl) {

	StopWebSocket();
}

void OnTermination(client* c, websocketpp::connection_hdl hdl) {

	StopWebSocket();
}

void RecvMessage(client* wsclient, websocketpp::connection_hdl hdl, message_ptr msg) {

	string message = msg->get_payload();
	try
	{
		const string  decode = websocketpp::base64_decode(message);

		neb::CJsonObject json(decode);
		int code = 0;
		string callback = "";
		json.Get("Callback", callback);
		if (!json.Get("Code", code)) return;
		neb::CJsonObject  respons;
		string WxId = "";
		json.Get("WxId", WxId);
		string FromWxId = "";
		json.Get("FromWxId", FromWxId);
		string Message = "";
		json.Get("Message", Message);
		string FilePath = "";
		json.Get("FilePath", FilePath);
		string ChatRoomId = "";
		json.Get("ChatRoomId", ChatRoomId);
		string ChatRoomName = "";
		json.Get("ChatRoomName", ChatRoomName);
		string MemberId = "";
		json.Get("MemberId", MemberId);
		string MemberWxId = "";
		json.Get("MemberWxId", MemberWxId);
		string MemberNick = "";
		json.Get("MemberNick", MemberNick);
		string RecverWxid = "";
		json.Get("RecverWxid", RecverWxid);
		string XML = "";
		json.Get("XML", XML);
		string Announcement = "";
		json.Get("Announcement", Announcement);
		string RunSqlStr = "";
		json.Get("RunSqlStr", RunSqlStr);

		switch (code)
		{
		case Cmd_Init:
			InitWeChat();
			break;
		case Cmd_CheckLoginStatus://检查登录状态:
			respons.Add("Status", CheckLogin());
			respons.Add("QrCodeStr", GetQrCodeStr());
			Send(Cmd_CheckLoginStatus, respons, callback);
			break;
		case Cmd_GetUser://获取用户信息
			if (CheckLogin()) {
				GetUser(callback);
			}
			else
			{
				Send(Cmd_GetUser, respons, callback, 1, L"当前微信还未登录。");
			}
			break;
		case Cmd_ContactList:
			GetContactList();
			break;
		case Cmd_SendTextMessage:
			SendTextMsg(EVString::a2w(WxId.c_str()), EVString::a2w(Message.c_str()));
			break;
		case Cmd_SendFileMessage:
			SendAttachMsg(EVString::a2w(WxId.c_str()), EVString::a2w(FilePath.c_str()));
			break;
		case Cmd_SendImageMessage:
			SendImageMsg(EVString::a2w(WxId.c_str()), EVString::a2w(FilePath.c_str()));
			break;
		case Cmd_SendAtMsg:
			SendChatroomAtMsg(EVString::a2w(ChatRoomId.c_str()), EVString::a2w(MemberId.c_str()), EVString::a2w(MemberNick.c_str()), EVString::a2w(Message.c_str()));
			break;
		case Cmd_SendXmlCard:
			SendXmlCard(EVString::a2w(RecverWxid.c_str()), EVString::a2w(XML.c_str()));
			break;
		case Cmd_SendXmlArticle:
			SendXmlArticle(EVString::a2w(RecverWxid.c_str()), EVString::a2w(FromWxId.c_str()), EVString::a2w(XML.c_str()));
			break;
		case Cmd_SetRoomName:
			SetChatoomName(EVString::a2w(ChatRoomId.c_str()), EVString::a2w(ChatRoomName.c_str()));
			break;
		case Cmd_SetChatRoomAnnouncement:
			SetChatroomAnnouncement(EVString::a2w(ChatRoomId.c_str()), EVString::a2w(Announcement.c_str()));
			break;
		case Cmd_AddChatRoomMember:
			AddChatroomMember(EVString::a2w(ChatRoomId.c_str()), EVString::a2w(WxId.c_str()));
			break;
		case Cmd_DeleteChatRoomMember:
			DeleteChatroomMember(EVString::a2w(ChatRoomId.c_str()), EVString::a2w(MemberWxId.c_str()));
			break;
		case Cmd_GetChatRoomMember:
			GetChatroomUser(EVString::a2w(ChatRoomId.c_str()), callback);
			break;
		case Cmd_QuitChatRoom:
			QuitChatRoom(EVString::a2w(ChatRoomId.c_str()));
			break;
		case Cmd_GotoQrCode:
			GotoQrCode();
			break;
		case Cmd_LogoutWeChat:
			LogoutWeChat();
			break;
		case Cmd_RunSql:
			char* sqlErrmsg = NULL;
			RunSql("MicroMsg.db", RunSqlStr, RunSqlSendCallBack, sqlErrmsg);
		}
		json.Clear();
	}
	catch (const std::exception& ex)
	{
		ostringstream ostr;
		ostr << "Source:" << "RecvMessage" << "		" << "Message:" << message.c_str() << "		" << "Exception:" << ex.what() << endl;
		WriteError(ostr.str());
	}
	catch (...) {
		ostringstream ostr;
		ostr << "Source:" << "RecvMessage" << "		" << "Message:" << message.c_str() << "		" << "Exception:" << "未知异常" << endl;
		WriteError(ostr.str());
	}
}


void Send(int code, neb::CJsonObject data, string callback, int err, wstring msg) {
	try
	{
		neb::CJsonObject json;
		json.Add("Code", code);
		json.Add("Error", err);
		json.Add("Message", EVString::w2a(msg.c_str()));
		json.Add("Pid", (long)GetCurrentProcessId());
		json.Add("Callback", callback);
		json.Add("Data", data);
		string data = json.ToString();
		string tmp = websocketpp::base64_encode(reinterpret_cast<const unsigned char*>(data.c_str()), data.length());

		websocketpp::lib::error_code ec;

		wsClient.send(con, tmp, websocketpp::frame::opcode::text, ec);
		if (ec) {
			WriteError("发送数据失败：" + ec.message());
		}
		data.clear();
		json.Clear();
	}
	catch (const std::exception& ex)
	{
		ostringstream ostr;
		ostr << "Source:" << "Send" << "		" << "Data:" << data.ToString() << "		" << "Exception:" << ex.what() << endl;
		WriteError(ostr.str());
	}
	catch (...) {
		ostringstream ostr;
		ostr << "Source:" << "Send" << "		" << "Data:" << data.ToString() << "		" << "Exception:" << "未知异常" << endl;
		WriteError(ostr.str());
	}
}