namespace TB_OMOK
{
    partial class Form1
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.Login = new System.Windows.Forms.Button();
            this.Logout = new System.Windows.Forms.Button();
            this.ChList = new System.Windows.Forms.ListBox();
            this.Join = new System.Windows.Forms.Button();
            this.Chat = new System.Windows.Forms.Label();
            this.SendMsg = new System.Windows.Forms.Button();
            this.ChatInput = new System.Windows.Forms.TextBox();
            this.LoginBox = new System.Windows.Forms.TextBox();
            this.ID = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.ChatInfo = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.CheckBoard = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            // 
            // Login
            // 
            this.Login.Location = new System.Drawing.Point(164, 6);
            this.Login.Name = "Login";
            this.Login.Size = new System.Drawing.Size(75, 25);
            this.Login.TabIndex = 0;
            this.Login.Text = "Login";
            this.Login.UseVisualStyleBackColor = true;
            this.Login.Click += new System.EventHandler(this.button1_Click);
            // 
            // Logout
            // 
            this.Logout.Location = new System.Drawing.Point(245, 6);
            this.Logout.Name = "Logout";
            this.Logout.Size = new System.Drawing.Size(75, 25);
            this.Logout.TabIndex = 1;
            this.Logout.Text = "Logout";
            this.Logout.UseVisualStyleBackColor = true;
            this.Logout.Click += new System.EventHandler(this.button2_Click);
            // 
            // ChList
            // 
            this.ChList.FormattingEnabled = true;
            this.ChList.ItemHeight = 15;
            this.ChList.Location = new System.Drawing.Point(441, 34);
            this.ChList.Name = "ChList";
            this.ChList.Size = new System.Drawing.Size(75, 124);
            this.ChList.TabIndex = 2;
            // 
            // Join
            // 
            this.Join.Location = new System.Drawing.Point(444, 165);
            this.Join.Name = "Join";
            this.Join.Size = new System.Drawing.Size(75, 23);
            this.Join.TabIndex = 3;
            this.Join.Text = "Join";
            this.Join.UseVisualStyleBackColor = true;
            this.Join.Click += new System.EventHandler(this.Join_Click);
            // 
            // Chat
            // 
            this.Chat.AutoSize = true;
            this.Chat.Location = new System.Drawing.Point(444, 218);
            this.Chat.Name = "Chat";
            this.Chat.Size = new System.Drawing.Size(37, 15);
            this.Chat.TabIndex = 4;
            this.Chat.Text = "Chat";
            this.Chat.Click += new System.EventHandler(this.Chat_Click);
            // 
            // SendMsg
            // 
            this.SendMsg.Location = new System.Drawing.Point(662, 413);
            this.SendMsg.Name = "SendMsg";
            this.SendMsg.Size = new System.Drawing.Size(81, 23);
            this.SendMsg.TabIndex = 5;
            this.SendMsg.Text = "SendMsg";
            this.SendMsg.UseVisualStyleBackColor = true;
            // 
            // ChatInput
            // 
            this.ChatInput.Location = new System.Drawing.Point(447, 413);
            this.ChatInput.Multiline = true;
            this.ChatInput.Name = "ChatInput";
            this.ChatInput.Size = new System.Drawing.Size(209, 23);
            this.ChatInput.TabIndex = 6;
            // 
            // LoginBox
            // 
            this.LoginBox.Location = new System.Drawing.Point(38, 6);
            this.LoginBox.Name = "LoginBox";
            this.LoginBox.Size = new System.Drawing.Size(120, 25);
            this.LoginBox.TabIndex = 7;
            this.LoginBox.TextChanged += new System.EventHandler(this.LoginBox_TextChanged);
            // 
            // ID
            // 
            this.ID.AutoSize = true;
            this.ID.Location = new System.Drawing.Point(12, 9);
            this.ID.Name = "ID";
            this.ID.Size = new System.Drawing.Size(20, 15);
            this.ID.TabIndex = 8;
            this.ID.Text = "ID";
            this.ID.Click += new System.EventHandler(this.label1_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(435, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(83, 15);
            this.label1.TabIndex = 9;
            this.label1.Text = "ChannelList";
            this.label1.Click += new System.EventHandler(this.label1_Click_1);
            // 
            // ChatInfo
            // 
            this.ChatInfo.Location = new System.Drawing.Point(447, 236);
            this.ChatInfo.Multiline = true;
            this.ChatInfo.Name = "ChatInfo";
            this.ChatInfo.Size = new System.Drawing.Size(296, 171);
            this.ChatInfo.TabIndex = 10;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(532, 30);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(206, 178);
            this.textBox1.TabIndex = 11;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(530, 12);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(51, 15);
            this.label2.TabIndex = 12;
            this.label2.Text = "IO Info";
            // 
            // CheckBoard
            // 
            this.CheckBoard.Location = new System.Drawing.Point(15, 88);
            this.CheckBoard.Margin = new System.Windows.Forms.Padding(0);
            this.CheckBoard.Name = "CheckBoard";
            this.CheckBoard.Size = new System.Drawing.Size(400, 428);
            this.CheckBoard.TabIndex = 13;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(782, 533);
            this.Controls.Add(this.CheckBoard);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.ChatInfo);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ID);
            this.Controls.Add(this.LoginBox);
            this.Controls.Add(this.ChatInput);
            this.Controls.Add(this.SendMsg);
            this.Controls.Add(this.Chat);
            this.Controls.Add(this.Join);
            this.Controls.Add(this.ChList);
            this.Controls.Add(this.Logout);
            this.Controls.Add(this.Login);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.Button Login;
        private System.Windows.Forms.Button Logout;
        private System.Windows.Forms.ListBox ChList;
        private System.Windows.Forms.Button Join;
        private System.Windows.Forms.Label Chat;
        private System.Windows.Forms.Button SendMsg;
        private System.Windows.Forms.TextBox ChatInput;
        private System.Windows.Forms.TextBox LoginBox;
        private System.Windows.Forms.Label ID;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox ChatInfo;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel CheckBoard;
    }
}

