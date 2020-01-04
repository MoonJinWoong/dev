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
        int margin = 10;
        int 눈Size = 20; // gridSize
        int 돌Size = 18; // stoneSize


        Graphics myGraphic;
        Pen   myPen;
        Brush whiteBrush,blackBrush;

        public Form1()
        {
            InitializeComponent();
            this.Text = "Test Omok"; // app 제목 
            this.CheckBoard.BackColor = Color.Orange;   // 패널 색

            myPen = new Pen(Color.Black);
            blackBrush = new SolidBrush(Color.Black);
            whiteBrush = new SolidBrush(Color.White);

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

            // 세로선
            for (int i = 0; i < 15; i++)
            {
                myGraphic.DrawLine(myPen, 
                  new Point(margin + i * 눈Size, margin),
                  new Point(margin + i * 눈Size, margin + 15 * 눈Size));
            }

            // 가로선 15개
            for (int i = 0; i < 15; i++)
            {
                myGraphic.DrawLine(myPen, 
                  new Point(margin, margin + i * 눈Size),
                  new Point(margin + 15 * 눈Size, margin + i * 눈Size));
            }

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
