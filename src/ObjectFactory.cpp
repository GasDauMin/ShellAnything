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

#include "ObjectFactory.h"

#include "shellanything/Configuration.h"
#include "shellanything/Menu.h"
#include "shellanything/Validator.h"
#include "shellanything/ActionClipboard.h"
#include "shellanything/ActionExecute.h"
#include "shellanything/ActionStop.h"
#include "shellanything/ActionFile.h"
#include "shellanything/ActionPrompt.h"
#include "shellanything/ActionProperty.h"
#include "shellanything/ActionOpen.h"
#include "shellanything/ActionMessage.h"

#include "rapidassist/strings.h"
#include "rapidassist/unicode.h"

using namespace tinyxml2;

namespace shellanything
{
  static const std::string NODE_MENU = "menu";
  static const std::string NODE_ICON = "icon";
  static const std::string NODE_VALIDITY = "validity";
  static const std::string NODE_VISIBILITY = "visibility";
  static const std::string NODE_DEFAULTSETTINGS = "default";
  static const std::string NODE_ACTION_CLIPBOARD = "clipboard";
  static const std::string NODE_ACTION_EXEC = "exec";
  static const std::string NODE_ACTION_STOP = "stop";
  static const std::string NODE_ACTION_FILE = "file";
  static const std::string NODE_ACTION_PROMPT = "prompt";
  static const std::string NODE_ACTION_PROPERTY = "property";
  static const std::string NODE_ACTION_OPEN = "open";
  static const std::string NODE_ACTION_MESSAGE = "message";

  ObjectFactory::ObjectFactory()
  {
  }

  ObjectFactory::~ObjectFactory()
  {
  }

  ObjectFactory & ObjectFactory::GetInstance()
  {
    static ObjectFactory _instance;
    return _instance;
  }

  typedef std::vector<const XMLElement *> ElementPtrList;
  ElementPtrList GetChildNodes(const XMLElement* element, const std::string & name)
  {
    ElementPtrList elements;

    if (!element)
      return elements;

    const XMLElement* current = element->FirstChildElement(name.c_str());
    while (current)
    {
      //found a new node
      elements.push_back(current);

      //next node
      current = current->NextSiblingElement(name.c_str());
    }

    return elements;
  }

  bool ParseAttribute(const XMLElement* element, const char * attr_name, bool is_optional, bool allow_empty_values, String & attr_value, String & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return false;
    }

    attr_value = "";

    const XMLAttribute * attr_node = element->FindAttribute(attr_name);
    if (is_optional && !attr_node)
    {
      //failed parsing but its not an error
      return false;
    }
    else if (!attr_node)
    {
      std::string temp;
      temp = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is missing attribute '" + attr_name + "'.";
      error = temp.c_str();
      return false;
    }

    attr_value = attr_node->Value();

    if (!allow_empty_values && attr_value.empty())
    {
      std::string temp;
      temp = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " have attribute '" + attr_name + "' value empty.";
      error = temp.c_str();
      return false;
    }

    return true;
  }

  bool ParseAttribute(const XMLElement* element, const char * attr_name, bool is_optional, bool allow_empty_values, int & attr_value, String & error)
  {
    std::string str_value;
    String s;
    if (!ParseAttribute(element, attr_name, is_optional, allow_empty_values, s, error))
      return false; //error is already set
    str_value = s.c_str();

    //convert string to int
    int int_value = -1;
    if (!ra::strings::Parse(str_value, int_value))
    {
      //failed parsing
      std::string temp;
      temp << "Failed parsing attribute '" << attr_name << "' of node '" << element->Name() << "'.";
      error = temp.c_str();
      return false;
    }

    //valid
    attr_value = int_value;
    return true;
  }

  Validator * ObjectFactory::ParseValidator(const tinyxml2::XMLElement * element, String & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    if (NODE_VALIDITY != element->Name() && NODE_VISIBILITY != element->Name() && NODE_ACTION_STOP != element->Name())
    {
      std::string temp;
      temp = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is not a <validity> or <visibility> node";
      error = temp.c_str();
      return NULL;
    }

    Validator * validator = new Validator();

    //parse class
    String class_;
    if (ParseAttribute(element, "class", true, true, class_, error))
    {
      if (!class_.empty())
      {
        validator->SetClass(class_);
      }
    }

    //parse pattern
    String pattern;
    if (ParseAttribute(element, "pattern", true, true, pattern, error))
    {
      if (!pattern.empty())
      {
        validator->SetPattern(pattern);
      }
    }

    //parse exprtk
    String exprtk;
    if (ParseAttribute(element, "exprtk", true, true, exprtk, error))
    {
      if (!exprtk.empty())
      {
        validator->SetExprtk(exprtk);
      }
    }

    //parse maxfiles
    int maxfiles = -1;
    if (ParseAttribute(element, "maxfiles", true, true, maxfiles, error))
    {
      validator->SetMaxFiles(maxfiles);
    }

    //parse maxfolders
    int maxfolders = -1;
    if (ParseAttribute(element, "maxfolders", true, true, maxfolders, error))
    {
      validator->SetMaxDirectories(maxfolders);
    }

    //parse fileextensions
    String fileextensions;
    if (ParseAttribute(element, "fileextensions", true, true, fileextensions, error))
    {
      if (!fileextensions.empty())
      {
        validator->SetFileExtensions(fileextensions);
      }
    }

    //parse exists
    String exists;
    if (ParseAttribute(element, "exists", true, true, exists, error))
    {
      if (!exists.empty())
      {
        validator->SetFileExists(exists);
      }
    }

    //parse properties
    String properties;
    if (ParseAttribute(element, "properties", true, true, properties, error))
    {
      if (!properties.empty())
      {
        validator->SetProperties(properties);
      }
    }

    //parse inverse
    String inverse;
    if (ParseAttribute(element, "inverse", true, true, inverse, error))
    {
      if (!inverse.empty())
      {
        validator->SetInserve(inverse);
      }
    }

    //parse istrue
    String istrue;
    if (ParseAttribute(element, "istrue", true, true, istrue, error))
    {
      if (!istrue.empty())
      {
        validator->SetIsTrue(istrue);
      }
    }

    //parse isfalse
    String isfalse;
    if (ParseAttribute(element, "isfalse", true, true, isfalse, error))
    {
      if (!isfalse.empty())
      {
        validator->SetIsFalse(isfalse);
      }
    }

    //parse isempty
    String isempty;
    if (ParseAttribute(element, "isempty", true, true, isempty, error))
    {
      if (!isempty.empty())
      {
        validator->SetIsEmpty(isempty);
      }
    }

    //success
    return validator;
  }

  Action * ObjectFactory::ParseAction(const XMLElement* element, String & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    //temporary parsed attribute values
    String tmp_str;
    int tmp_int = -1;

    if (NODE_ACTION_CLIPBOARD == element->Name())
    {
      ActionClipboard * action = new ActionClipboard();

      //parse value
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "value", false, true, tmp_str, error))
      {
        action->SetValue(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_EXEC == element->Name())
    {
      ActionExecute * action = new ActionExecute();

      //parse path
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "path", false, true, tmp_str, error))
      {
        action->SetPath(tmp_str);
      }

      //parse arguments
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "arguments", true, true, tmp_str, error))
      {
        action->SetArguments(tmp_str);
      }

      //parse basedir
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "basedir", true, true, tmp_str, error))
      {
        action->SetBaseDir(tmp_str);
      }

      //parse verb
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "verb", true, true, tmp_str, error))
      {
        action->SetVerb(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_STOP == element->Name())
    {
      ActionStop* action = new ActionStop();

      //parse like a Validator
      Validator* validator = ObjectFactory::GetInstance().ParseValidator(element, error);
      if (validator == NULL)
      {
        delete action;
        return NULL;
      }

      action->SetValidator(validator);

      //done parsing
      return action;
    }
    else if (NODE_ACTION_FILE == element->Name())
    {
      ActionFile * action = new ActionFile();

      //parse path
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "path", false, true, tmp_str, error))
      {
        action->SetPath(tmp_str);
      }

      //parse text
      const char * text = element->GetText();
      if (text)
      {
        action->SetText(text);
      }

      //parse encoding
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "encoding", true, true, tmp_str, error))
      {
        action->SetEncoding(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_PROMPT == element->Name())
    {
      ActionPrompt * action = new ActionPrompt();

      //parse name
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "name", false, true, tmp_str, error))
      {
        action->SetName(tmp_str);
      }

      //parse title
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "title", false, true, tmp_str, error))
      {
        action->SetTitle(tmp_str);
      }

      //parse default
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "default", true, true, tmp_str, error))
      {
        action->SetDefault(tmp_str);
      }

      //parse type
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "type", true, true, tmp_str, error))
      {
        action->SetType(tmp_str);
      }

      //parse valueyes
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "valueyes", true, true, tmp_str, error))
      {
        action->SetValueYes(tmp_str);
      }

      //parse valueno
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "valueno", true, true, tmp_str, error))
      {
        action->SetValueNo(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_PROPERTY == element->Name())
    {
      ActionProperty * action = new ActionProperty();

      //parse name
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "name", false, true, tmp_str, error))
      {
        action->SetName(tmp_str);
      }

      //parse value
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "value", true, true, tmp_str, error))
      {
        action->SetValue(tmp_str);
      }

      //parse exprtk
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "exprtk", true, true, tmp_str, error))
      {
        action->SetExprtk(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_OPEN == element->Name())
    {
      ActionOpen * action = new ActionOpen();

      //parse path
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "path", false, true, tmp_str, error))
      {
        action->SetPath(tmp_str);
      }

      //done parsing
      return action;
    }
    else if (NODE_ACTION_MESSAGE == element->Name())
    {
      ActionMessage * action = new ActionMessage();

      //parse title
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "title", false, true, tmp_str, error))
      {
        action->SetTitle(tmp_str);
      }

      //parse caption
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "caption", false, true, tmp_str, error))
      {
        action->SetCaption(tmp_str);
      }

      //parse icon
      tmp_str = "";
      tmp_int = -1;
      if (ParseAttribute(element, "icon", true, true, tmp_str, error))
      {
        action->SetIcon(tmp_str);
      }

      //done parsing
      return action;
    }
    else
    {
      std::string temp;
      temp = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
      error = temp.c_str();
      return NULL;
    }

    //invalid
    return NULL;
  }

  Menu * ObjectFactory::ParseMenu(const XMLElement* element, String & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    std::string xml_name = element->Name();
    if (xml_name != NODE_MENU)
    {
      std::string temp;
      temp = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
      error = temp.c_str();
      return NULL;
    }

    //at this step the <menu> is valid
    Menu * menu = new Menu();

    //parse separator
    String menu_separator;
    bool have_separator = ParseAttribute(element, "separator", true, true, menu_separator, error);
    bool separator_parsed = false;
    if (have_separator)
    {
      //try to parse this menu separator as a boolean
      bool is_horizontal_separator = false;
      separator_parsed = ra::strings::Parse(menu_separator.c_str(), is_horizontal_separator);
      if (separator_parsed && is_horizontal_separator)
      {
        menu->SetSeparator(true);
        return menu;
      }

      //try to parse as a string
      menu_separator = ra::strings::Lowercase(menu_separator.c_str()).c_str();
      if (menu_separator == "horizontal")
      {
        menu->SetSeparator(true);
        return menu;
      }
      else if (menu_separator == "vertical" || menu_separator == "column")
      {
        menu->SetColumnSeparator(true);
        return menu;
      }
    }

    //parse name
    String menu_name;
    if (!ParseAttribute(element, "name", false, false, menu_name, error))
    {
      delete menu;
      return NULL;
    }
    menu->SetName(menu_name);

    //parse description
    String menu_desc;
    if (!ParseAttribute(element, "description", true, true, menu_desc, error))
    {
      menu->SetDescription(menu_desc);
    }

    //parse icon
    String icon_path;
    if (ParseAttribute(element, "icon", true, true, icon_path, error))
    {
      Icon icon;
      icon.SetPath(icon_path);
      menu->SetIcon(icon);
    }

    //parse maxlength
    String maxlength_str;
    if (ParseAttribute(element, "maxlength", true, true, maxlength_str, error))
    {
      int maxlength = 0;
      if (ra::strings::Parse(maxlength_str.c_str(), maxlength) && maxlength > 0)
      {
        menu->SetNameMaxLength(maxlength);
      }
    }

    ElementPtrList elements; //temporary xml element containers

    //find <validity> node under <menu>
    elements = GetChildNodes(element, NODE_VALIDITY);
    for(size_t i=0; i<elements.size(); i++)
    {
      Validator * validator = ObjectFactory::GetInstance().ParseValidator(elements[i], error);
      if (validator == NULL)
      {
        delete menu;
        return NULL;
      }

      //add the new validator
      menu->AddValidity(validator);
    }

    //find <visibility> node under <menu>
    elements = GetChildNodes(element, NODE_VISIBILITY);
    for(size_t i=0; i<elements.size(); i++)
    {
      Validator * validator = ObjectFactory::GetInstance().ParseValidator(elements[i], error);
      if (validator == NULL)
      {
        delete menu;
        return NULL;
      }

      //add the new validator
      menu->AddVisibility(validator);
    }

    //find <actions> node under <menu>
    const XMLElement* xml_actions = element->FirstChildElement("actions");
    if (xml_actions)
    {
      //actions must be read in order.

      //find <clipboard>, <exec>, <prompt>, <property> or <open> nodes under <actions>
      const XMLElement* xml_action = xml_actions->FirstChildElement();
      while (xml_action)
      {
        //found a new action node
        Action * action = ObjectFactory::GetInstance().ParseAction(xml_action, error);
        if (action == NULL)
        {
          delete menu;
          return NULL;
        }

        //add the new action node
        menu->AddAction(action);

        //next action node
        xml_action = xml_action->NextSiblingElement();
      }
    }

    //find <menu> node under <menu>
    elements = GetChildNodes(element, NODE_MENU);
    for(size_t i=0; i<elements.size(); i++)
    {
      Menu * submenu = ObjectFactory::GetInstance().ParseMenu(elements[i], error);
      if (submenu == NULL)
      {
        delete menu;
        return NULL;
      }
      menu->AddMenu(submenu);
    }

    //find <icon> node under <menu>
    elements = GetChildNodes(element, "icon");
    for(size_t i=0; i<elements.size(); i++)
    {
      Icon icon;
      if (!ObjectFactory::GetInstance().ParseIcon(elements[i], icon, error))
      {
        //failed icon parsing
        delete menu;
        return NULL;
      }
      menu->SetIcon(icon);
    }

    return menu;
  }

  bool ObjectFactory::ParseIcon(const tinyxml2::XMLElement * element, Icon & icon, String & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return false;
    }

    String xml_name = element->Name();
    if (NODE_ICON != xml_name.c_str())
    {
      std::string temp;
      temp = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
      error = temp.c_str();
      return NULL;
    }

    //parse path
    String icon_path;
    bool hasPath = ParseAttribute(element, "path", true, true, icon_path, error);

    //parse fileextension
    String icon_fileextension;
    bool hasFileExtension = ParseAttribute(element, "fileextension", true, true, icon_fileextension, error);
    
    if (!hasPath && !hasFileExtension)
    {
      //failed parsing
      return false;
    }

    Icon result;
    if (hasPath)
      result.SetPath(icon_path);
    if (hasFileExtension)
      result.SetFileExtension(icon_fileextension);

    //parse index
    int icon_index = -1;
    if (ParseAttribute(element, "index", true, true, icon_index, error))
    {
      result.SetIndex(icon_index);
    }

    //success
    icon = result;
    return true;
  }

  DefaultSettings * ObjectFactory::ParseDefaults(const XMLElement* element, String & error)
  {
    if (element == NULL)
    {
      error = "XMLElement is NULL";
      return NULL;
    }

    String xml_name = element->Name();
    if (NODE_DEFAULTSETTINGS != xml_name.c_str())
    {
      std::string temp;
      temp = "Node '" + std::string(element->Name()) + "' at line " + ra::strings::ToString(element->GetLineNum()) + " is an unknown type.";
      error = temp.c_str();
      return NULL;
    }

    DefaultSettings * defaults = new DefaultSettings();

    ElementPtrList elements; //temporary xml element containers

    //find <property> node under <default>
    elements = GetChildNodes(element, NODE_ACTION_PROPERTY);
    for(size_t i=0; i<elements.size(); i++)
    {
      const tinyxml2::XMLElement * element = elements[i];

      //found a new action node
      Action * abstract_action = ObjectFactory::GetInstance().ParseAction(element, error);
      if (abstract_action)
      {
        //filter out all type of actions except ActionProperty actions
        ActionProperty * property_action = dynamic_cast<ActionProperty *>(abstract_action);
        if (property_action != NULL)
        {
          //add the new action node
          defaults->AddAction(property_action);
        }
        else
        {
          delete abstract_action;
        }
      }
    }

    //do not return a DefaultSettings instance if empty.
    if (defaults->GetActions().empty())
    {
      delete defaults;
      return NULL;
    }

    return defaults;
  }

} //namespace shellanything
