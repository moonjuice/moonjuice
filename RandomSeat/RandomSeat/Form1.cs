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
            //隨機亂排
            while (students.Count > 0)
            {
                Random Counter = new Random(Guid.NewGuid().GetHashCode());
                int index = Counter.Next(students.Count);
                seats.Add(students.ElementAt(index));
                students.RemoveAt(index);
            }
            //存檔
            //設定必要的物件

            Microsoft.Office.Interop.Excel.Application oXL = new Microsoft.Office.Interop.Excel.Application();

            Microsoft.Office.Interop.Excel.Workbook oWB;

            Microsoft.Office.Interop.Excel.Worksheet oSheet;

            Microsoft.Office.Interop.Excel.Range oRng;



            oXL.Visible = true;



            //產生一個Workbook物件，並加入Application

            oWB = oXL.Workbooks.Add(Missing.Value);



            //設定工作表

            oSheet = (Microsoft.Office.Interop.Excel.Worksheet)oWB.ActiveSheet;



            //在工作表的特定儲存格，設定內容

            //oSheet.Cells[1, 1] = firstLineTitle;

            //oSheet.Cells[2, 1] = secondLineTitle;



            //在特定範圍設定儲存格內容

            //oSheet.get_Range("A4", "I4").Value2 = header;



            System.Data.DataTable dt;//....取得資料表



            //從第五行開始，設定第一欄和第二欄的內容

            //資料是從某資料表取出

            int row = 5;

            for (int counter = 0; counter < dt.Rows.Count; counter++)
            {

                oSheet.Cells[row, 1] = dt.Rows[counter]["XYZ"].ToString();

                oSheet.Cells[row, 2] = dt.Rows[counter]["ABC"].ToString();



                row++;

            }



            //設定為按照內容自動調整欄寬

            oRng = oSheet.get_Range("A5", "A" + row.ToString());

            oRng.EntireColumn.AutoFit();



            //設定為置中

            oRng = oSheet.get_Range("B5", "B" + row.ToString());

            oRng.EntireColumn.HorizontalAlignment = Microsoft.Office.Interop.Excel.XlHAlign.xlHAlignCenter;



            //設定為讓使用者從程式的開始執行後

            //就可以操作Excel，例如另存新檔到不同路徑

            oXL.Visible = true;

            oXL.UserControl = true;



            //存檔

            //在這裡只設定檔案名稱(含路徑)即可

            oWB.SaveAs("fileName", Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing,

                Microsoft.Office.Interop.Excel.XlSaveAsAccessMode.xlShared, Type.Missing, Type.Missing, Type.Missing, Type.Missing, Type.Missing);


            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.Filter = "Excel|*.xls";

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                //if ((myStream = saveFileDialog1.OpenFile()) != null)
                //{
                //    // Code to write the stream goes here.
                //    myStream.Close();
                //}
            }
        }
    }
}
