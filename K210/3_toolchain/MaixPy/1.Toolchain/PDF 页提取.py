# PDF 页面提取

from PyPDF2 import PdfFileWriter, PdfFileReader


start_page = 36  # 开始页
end_page = 48  # 截止页
input_file = "mpy v1.2.pdf"
output_file = "output.pdf"

output = PdfFileWriter()
pdf_file = PdfFileReader(open(input_file, "rb"))
pdf_pages_len = pdf_file.getNumPages()

# 保存input.pdf中的1-5页到output.pdf
for i in range(start_page, end_page):
    output.addPage(pdf_file.getPage(i))

outputStream = open(output_file, "wb")
output.write(outputStream)
