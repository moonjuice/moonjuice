using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Reflection;
using NPOI.HSSF.UserModel;

namespace RandomSeat
{
    public partial class Form1 : Form
    {
        List<Student> students;
        List<Student> seats;
        int col;
        public Form1()
        {
            InitializeComponent();
            students = new List<Student>();
            seats = new List<Student>();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Stream myStream;
            OpenFileDialog input = new OpenFileDialog();

            if (input.ShowDialog() == DialogResult.OK)
            {
                if ((myStream = input.OpenFile()) != null)
                {
                    using (StreamReader sr = new StreamReader(myStream))
                    {
                        String line;
                        while ((line = sr.ReadLine()) != null)
                        {
                            Student s = new Student();
                            if (line.Contains("//"))
                                continue;
                            else if (line.Contains(","))
                            {
                                string[] temp = line.Split(',');
                                s.ID = temp[0];
                                s.Name = temp[1];
                            }
                            else
                                s.Name = line;
                            students.Add(s);
                        }
                    }
                    myStream.Close();
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (students.Count == 0 && seats.Count != 0)
                students = new List<Student>(seats);
            seats.Clear();
            //隨機亂排
            while (students.Count > 0)
            {
                Random Counter = new Random(Guid.NewGuid().GetHashCode());
                int index = Counter.Next(students.Count);
                seats.Add(students.ElementAt(index));
                students.RemoveAt(index);
            }
            //建立Excel
            HSSFWorkbook hssfworkbook = new HSSFWorkbook();
            HSSFSheet sheet = (HSSFSheet)hssfworkbook.CreateSheet("座位表");
            sheet.CreateRow(0).CreateCell(0).SetCellValue("黑板");            
            sheet.CreateRow(1).CreateCell(0).SetCellValue("講台");

            //合併單元格(第一行第一列到第一行第col列)
            sheet.AddMergedRegion(new NPOI.SS.Util.CellRangeAddress(0, (short)0, 0, (short)col-1));
            sheet.AddMergedRegion(new NPOI.SS.Util.CellRangeAddress(1, (short)1, 0, (short)col-1));
            sheet.GetRow(0).GetCell(0).CellStyle.Alignment = NPOI.SS.UserModel.HorizontalAlignment.CENTER;//文字置中
            sheet.GetRow(1).GetCell(0).CellStyle.Alignment = NPOI.SS.UserModel.HorizontalAlignment.CENTER;//文字置中

            int i = 2, j = 0;
            sheet.CreateRow(i);
            foreach (Student s in seats)
            {
                if (s.ID.Equals(""))
                    sheet.GetRow(i).CreateCell(j).SetCellValue(s.Name);
                else
                {
                    sheet.GetRow(i).CreateCell(j).SetCellValue(s.ID + "\n" + s.Name);

                    HSSFCellStyle cs = (HSSFCellStyle)hssfworkbook.CreateCellStyle();
                    cs.WrapText = true;
                    sheet.GetRow(i).GetCell(j).CellStyle = cs;

                    //因為換行所以預設幫他Row的高度變成兩倍
                    sheet.GetRow(i).HeightInPoints = 2 * sheet.DefaultRowHeight / 20;
                }
                j++;
                if (j % col == 0)
                {
                    i++;
                    sheet.CreateRow(i);
                    j = 0;
                }
            }
            //存檔
            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.Filter = "Excel|*.xls";
            saveFileDialog1.FileName = "座位表";
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                FileStream file;
                file = new FileStream(saveFileDialog1.FileName, FileMode.Create);
                hssfworkbook.Write(file);
                file.Close();
            }

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            col = int.Parse(textBox1.Text.ToString());
        }
    }
}
