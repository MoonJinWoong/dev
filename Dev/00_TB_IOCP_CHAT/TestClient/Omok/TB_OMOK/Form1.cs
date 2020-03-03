using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TB_OMOK
{
    public partial class Form1 : Form
    {
        Graphics myGraphic;
        Pen   myPen;
        Brush whiteBrush,blackBrush;

        enum STATE { NONE = 0, WHITE = 1, BLACK = 2 };

        // 내 차례인가 ? 
        bool IsMyTurn = false;


        public Form1()
        {
            InitializeComponent();
            this.Text = "Test Omok"; // app 제목 
            this.CheckBoard.BackColor = Color.Goldenrod;   // 패널 색

            myPen = new Pen(Color.Black);
            blackBrush = new SolidBrush(Color.Black);
            whiteBrush = new SolidBrush(Color.White);

           // 패널 사이즈 세팅
           this.CheckBoard.Size = new Size(340, 340);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            DrawBoard();
        }
        private void DrawBoard()
        {
            // panel1에 Graphics 객체 생성
            myGraphic = CheckBoard.CreateGraphics();
            
            // 15 칸만 만들자..
            // 가로 좌표 20  ~ 320 
            // 세로 좌표 20 ~ 320
            for(int i = 1; i <= 16; ++i)
            {
                myGraphic.DrawLine(myPen, 20, i*20 , 320, i * 20);  // 가로
                myGraphic.DrawLine(myPen, i*20, 20, i*20, 320);  // 세로

            }


        }

        private void panel1_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            this.textBox1.Text = "asdf";

        }
        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

        private void Chat_Click(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void Join_Click(object sender, EventArgs e)
        {

        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void LoginBox_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
