/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#ifndef WIN32_CLIPBOARD_H
#define WIN32_CLIPBOARD_H

#include <vector>
#include <string>

namespace Win32Clipboard
{

  class Clipboard
  {
  private:
    Clipboard();
    virtual ~Clipboard();

  public:
    static Clipboard & GetInstance();

    //enums
    enum Format { FormatText, FormatUnicode, FormatImage, FormatBinary };
    enum DragDropType {DragDropCopy, DragDropCut};

    //typedefs
    typedef std::vector<std::string> StringVector;
    typedef std::vector<std::wstring> StringVectorUnicode;
    typedef std::string MemoryBuffer;

    //constants
    static const size_t NUM_FORMATS = 4;

    /// <summary>
    /// Clear the clipboard.
    /// </summary>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool Empty();

    /// <summary>
    /// Returns true if the clipboard is empty.
    /// </summary>
    /// <returns>Returns true if the clipboard is empty. Returns false otherwise.</returns>
    virtual bool IsEmpty();

    /// <summary>
    /// Query the clipboard to know if it Contains the given format.
    /// </summary>
    /// <param name="clipboard_format">The format to query.</param>
    /// <returns>Returns true if the clipboard constains a specific format.</returns>
    virtual bool Contains(Format clipboard_format);

    /// <summary>
    /// Assign the given text value to the clipboard.
    /// </summary>
    /// <param name="text">The text value to set to the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool SetText(const std::string & text);

    /// <summary>
    /// Provides the current text value of the clipboard.
    /// </summary>
    /// <param name="text">The output text value of the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool GetAsText(std::string & text);

    /// <summary>
    /// Assign the given unicode text value to the clipboard.
    /// </summary>
    /// <param name="text">The unicode text value to set to the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool SetTextUnicode(const std::wstring & text);

    /// <summary>
    /// Provides the current unicode text value of the clipboard.
    /// </summary>
    /// <param name="text">The output unicode text value of the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool GetAsTextUnicode(std::wstring & text);

    /// <summary>
    /// Assign the given binary data to the clipboard.
    /// </summary>
    /// <param name="buffer">The binary data to set to the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool SetBinary(const MemoryBuffer & buffer);

    /// <summary>
    /// Provides the current binary data of the clipboard.
    /// </summary>
    /// <param name="buffer">The output binary data of the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool GetAsBinary(MemoryBuffer & buffer);

    /// <summary>
    /// Assign the given file operation and list of files to the clipboard.
    /// Files are encoded in ANSI.
    /// </summary>
    /// <param name="type">The file operation to set to the clipboard.</param>
    /// <param name="files">The list of files, encoded in ansi, to set to the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool SetDragDropFiles(const DragDropType & type, const StringVector & files);

    /// <summary>
    /// Provides the current file operation and list of files set to the clipboard.
    /// Files are encoded in ANSI.
    /// </summary>
    /// <param name="type">The file operation set to the clipboard.</param>
    /// <param name="files">The list of files, encoded in ansi, set to the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool GetAsDragDropFiles(DragDropType & type, StringVector & files);

    /// <summary>
    /// Assign the given file operation and list of files to the clipboard.
    /// Files are encoded in UNICODE.
    /// </summary>
    /// <param name="type">The file operation to set to the clipboard.</param>
    /// <param name="files">The list of files, encoded in unicode, to set to the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool SetDragDropFilesUnicode(const DragDropType & type, const StringVectorUnicode & files);

    /// <summary>
    /// Provides the current file operation and list of files set to the clipboard.
    /// Files are encoded in UNICODE.
    /// </summary>
    /// <param name="type">The file operation set to the clipboard.</param>
    /// <param name="files">The list of files, encoded in unicode, set to the clipboard.</param>
    /// <returns>Returns true if the function is successful. Returns false otherwise.</returns>
    virtual bool GetAsDragDropFilesUnicode(DragDropType & type, StringVectorUnicode & files);

  };

} //namespace Win32Clipboard

#endif //WIN32_CLIPBOARD_H
