using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Microsoft.Office.Interop.Excel;
using System.Reflection;
using NPOI.HSSF.UserModel;

namespace RandomSeat
{
    public partial class Form1 : Form
    {
        List<Student> students;
        List<Student> seats;
        public Form1()
        {
            InitializeComponent();
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
                            if (line.Contains(","))
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
            int col = 10;
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
            int i = 0, j = 0;
            foreach (Student s in seats)
            {
                if (s.ID.Equals(""))
                    sheet.CreateRow(i).CreateCell(j).SetCellValue(s.Name);
                else
                    sheet.CreateRow(i).CreateCell(j).SetCellValue(s.ID + "\n" + s.Name);
                j++;
                if (j % col == 0)
                {
                    i++;
                    j++;
                }
            }
            //存檔
            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.Filter = "Excel|*.xls";

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                FileStream file;
                if (saveFileDialog1.FileName.Equals(""))
                    file = new FileStream("座位表.xls", FileMode.Create);
                else
                    file = new FileStream(saveFileDialog1.FileName, FileMode.Create);
                hssfworkbook.Write(file);
                file.Close();
            }

        }
    }
}
