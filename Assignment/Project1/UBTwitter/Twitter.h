#pragma once
#include "Mailslotmgr.h"
#include <msclr\marshal_cppstd.h>

namespace UBTwitter {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace System::Collections::Generic;
	/// <summary>
	/// Summary for MessageManagerWindow
	/// </summary>
	enum eThreadState
	{
		STARTING,
		RUNNING,
		STOPPING,
		STOPED
	};

	public ref class MessageManagerWindow : public System::Windows::Forms::Form
	{

		static int myMailBoxID = 0;
		eThreadState running_flag = STARTING;
		eThreadState client_running_flag = STARTING;
		MailslotMrg *mailbox = NULL;
		List<String^>^ MsgToSend = gcnew List<String^>();
		List<int>^ MsgToSendto = gcnew List<int>();
		static Random ^random = gcnew Random((int)System::DateTime().Now.Ticks);

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::Timer^  timer2;

	private: System::Windows::Forms::TextBox^  textMailBoxID;
	public:
		MessageManagerWindow(void)
		{
			InitializeComponent();

			mailbox = new MailslotMrg();
			myMailBoxID = mailbox->GetMailslotID();

			if (myMailBoxID != INVALID_MAILSLOT)
			{
				//removing this mailbox from the list. No sending message to self.
				MailBoxList->Items->RemoveAt(myMailBoxID);
				textMailBoxID->Text = myMailBoxID.ToString();
				running_flag = RUNNING;
				client_running_flag = RUNNING;
				DisplayMessage("Server Started");
				DisplayMessage("Client Started");
				TimerEnable(TRUE);
				Sleep(10);
			}
			else
			{
				running_flag = STOPED;
				client_running_flag = STOPED;
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MessageManagerWindow()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  OnSendMsg;
	private: System::Windows::Forms::ComboBox^  MailBoxList;
	private: System::Windows::Forms::RichTextBox^  History;

	protected:


	private: System::Windows::Forms::RichTextBox^  MessageText;
	private: System::Windows::Forms::Button^  ExitButton;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->OnSendMsg = (gcnew System::Windows::Forms::Button());
			this->MailBoxList = (gcnew System::Windows::Forms::ComboBox());
			this->History = (gcnew System::Windows::Forms::RichTextBox());
			this->MessageText = (gcnew System::Windows::Forms::RichTextBox());
			this->ExitButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textMailBoxID = (gcnew System::Windows::Forms::TextBox());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->timer2 = (gcnew System::Windows::Forms::Timer(this->components));
			this->SuspendLayout();
			// 
			// OnSendMsg
			// 
			this->OnSendMsg->Location = System::Drawing::Point(275, 280);
			this->OnSendMsg->Name = L"OnSendMsg";
			this->OnSendMsg->Size = System::Drawing::Size(75, 42);
			this->OnSendMsg->TabIndex = 0;
			this->OnSendMsg->Text = L"Send";
			this->OnSendMsg->UseVisualStyleBackColor = true;
			this->OnSendMsg->Click += gcnew System::EventHandler(this, &MessageManagerWindow::OnSendMsg_Click);
			// 
			// MailBoxList
			// 
			this->MailBoxList->FormattingEnabled = true;
			this->MailBoxList->Items->AddRange(gcnew cli::array< System::Object^  >(10) {
				L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7",
					L"8", L"9"
			});
			this->MailBoxList->Location = System::Drawing::Point(12, 25);
			this->MailBoxList->Name = L"MailBoxList";
			this->MailBoxList->Size = System::Drawing::Size(121, 21);
			this->MailBoxList->TabIndex = 1;
			// 
			// History
			// 
			this->History->Location = System::Drawing::Point(12, 64);
			this->History->Name = L"History";
			this->History->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::Vertical;
			this->History->Size = System::Drawing::Size(338, 128);
			this->History->TabIndex = 2;
			this->History->Text = L"";
			// 
			// MessageText
			// 
			this->MessageText->Location = System::Drawing::Point(12, 198);
			this->MessageText->Name = L"MessageText";
			this->MessageText->Size = System::Drawing::Size(338, 69);
			this->MessageText->TabIndex = 3;
			this->MessageText->Text = L"";
			// 
			// ExitButton
			// 
			this->ExitButton->Location = System::Drawing::Point(12, 280);
			this->ExitButton->Name = L"ExitButton";
			this->ExitButton->Size = System::Drawing::Size(75, 42);
			this->ExitButton->TabIndex = 4;
			this->ExitButton->Text = L"Quit";
			this->ExitButton->UseVisualStyleBackColor = true;
			this->ExitButton->Click += gcnew System::EventHandler(this, &MessageManagerWindow::ExitButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(274, 31);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(46, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"User ID:";
			// 
			// textMailBoxID
			// 
			this->textMailBoxID->Location = System::Drawing::Point(322, 25);
			this->textMailBoxID->Name = L"textMailBoxID";
			this->textMailBoxID->ReadOnly = true;
			this->textMailBoxID->Size = System::Drawing::Size(28, 20);
			this->textMailBoxID->TabIndex = 6;
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &MessageManagerWindow::timer1_Tick);
			// 
			// timer2
			// 
			this->timer2->Tick += gcnew System::EventHandler(this, &MessageManagerWindow::timer2_Tick);
			// 
			// MessageManagerWindow
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(362, 334);
			this->Controls->Add(this->textMailBoxID);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->ExitButton);
			this->Controls->Add(this->MessageText);
			this->Controls->Add(this->History);
			this->Controls->Add(this->MailBoxList);
			this->Controls->Add(this->OnSendMsg);
			this->Name = L"MessageManagerWindow";
			this->Text = L"Tweets";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &MessageManagerWindow::MessageManagerWindow_FormClosing);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MessageManagerWindow_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
		ExitButton_Click(sender, e);
	}

	private: System::Void OnSendMsg_Click(System::Object^  sender, System::EventArgs^  e) {
		String ^data = MessageText->Text->ToString();
		if (MailBoxList->SelectedIndex >= 0 && MailBoxList->SelectedItem->ToString()->Length > 0)
		{
			int box = Convert::ToInt32(MailBoxList->SelectedItem->ToString());
			if (box >= 0) {
				MsgToSend->Add(data);
				MsgToSendto->Add(box);
			}
		}
		MessageText->Clear();
	}

	private: System::Void ExitButton_Click(System::Object^  sender, System::EventArgs^  e) {
		TimerEnable(FALSE);
		if (client_running_flag == RUNNING)
			client_running_flag = STOPPING;
		if (running_flag == RUNNING)
		{
			running_flag = STOPPING;
			delete mailbox;
			mailbox = NULL;
		}
		Application::ExitThread();
	}

	public: Void DisplayMessage(String ^data)
	{
		System::DateTime time = System::DateTime().Now;
		String ^tmp = time.ToString("hh:mm:ss ");
		tmp += data + "\n";
		History->Text += tmp;
	}

	public: Void DisplayMessage(int to, String ^data)
	{
		System::DateTime time = System::DateTime().Now;
		String ^tmp = time.ToString("hh:mm:ss ");
		tmp += "To: " + to.ToString() + " => " + data + "\n";
		History->Text += tmp;
	}

			Void RunServer()
			{
				if (running_flag == RUNNING)
				{
					std::string tmp = mailbox->ReadTweets();
					if (tmp.length() > 0)         //We got something?
					{
						String ^data = msclr::interop::marshal_as<String^>(tmp);
						DisplayMessage(data);
						Invalidate();
					}
				}
			}

	Void RunClient()
	{
		if (client_running_flag == RUNNING)
		{
			if (MsgToSendto->Count > 0)
			{
				std::string tmp = msclr::interop::marshal_as<std::string>(MsgToSend[0]->ToString());
				int box = MsgToSendto[0];
				DisplayMessage(box, MsgToSend[0]->ToString());
				MsgToSendto->RemoveAt(0);
				MsgToSend->RemoveAt(0);
				for (int i = 0; i < 10; i++)
				{
					mailbox->WriteTweets(box, tmp);
					Sleep(5);
				}
			}
		}
	}

	void TimerEnable(BOOL flag)
	{
		this->timer1->Interval = 500;
		this->timer1->Enabled = flag;
		this->timer2->Interval = 500;
		this->timer2->Enabled = flag;
	}

	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e) {
		RunServer();
	}
	private: System::Void timer2_Tick(System::Object^  sender, System::EventArgs^  e) {
		RunClient();
	}
	};
}
