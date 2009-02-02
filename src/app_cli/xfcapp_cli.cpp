/***************************************************************************
 *   Copyright (C) 2009 by Tudor Pristavu   *
 *   clapautiuAtGmaliDotCom   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <getopt.h>

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>

#include "xfcapp.h"
#include "interface.h"
#include "xfc.h"
#include "misc.h"
#include "fs.h"
#include "plugins.h"

using namespace std;

Xfc gXfc;

XfcLogger gcLog; // cli logger

int printToStdout(unsigned int lDepth, std::string lPath, Xfc&,
                   xmlNodePtr lpNode, void *lpParam);
int findInTree(unsigned int lDepth, std::string lPath, Xfc&,
                xmlNodePtr lpNode, void *lpParam);
void initStuff();

std::vector<std::string> gSearchResultsPaths;
std::vector<xmlNodePtr> gSearchResultsNodes;

/// @return 0 - ok, 1 - exit
static int processCommand(std::vector<std::string> &rCmd);

static void cmdRenameDisk(std::vector<std::string> cmd);

static void cmdVerifyDisk(std::vector<std::string> cmd);



static void
displayUsage()
{
    cout<<"Usage: bitKatalog_cli [--verbose-level <level (0-3)>]  [--xfclib-verbose-level <level (0-3)>]";
    cout<<endl<<endl<<XFCAPP_NAME<<", version: "<<XFCAPP_VERSION<<endl<<endl;
}


/**
 * @return 0 = ok, <0 = should exit
 **/
static int
parseCmdLine(int argc, char **argv)
{
    int rc=0;
    int cc;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"help", 0, 0, 0},
            {"verbose-level", 1, 0, 0},
            {"xfclib-verbose-level", 1, 0, 0},
            {0, 0, 0, 0}
        };

        cc = getopt_long(argc, argv, "h", long_options, &option_index);
        if (cc == -1)
            break;

        switch (cc) {
        case 0:
            switch (option_index) {
            case 0: // --help
                displayUsage();
                rc=-1;
                break;

            case 1: // --verbose-level
                gcLog.setVerboseLevel((unsigned int)atoi(optarg));
                gcLog<<xfcInfo<<"verbose level is "<<gcLog.getVerboseLevel()<<eol;
                break;

            case 2: // --xfclib-verbose-level
                gLog.setVerboseLevel((unsigned int)atoi(optarg));
                gcLog<<xfcInfo<<"verbose level for xfclib is "<<gLog.getVerboseLevel()<<eol;
                break;
            }
            break;

        case 'h':
            displayUsage();
            rc=-1;
            break;

        case '?':
            rc=-1;
            break;
        }
    }
    if (optind < argc) {
            displayUsage();
            rc=-1;
    }
    return rc;
}

                
int main(int argc, char *argv[])
{
    std::vector<std::string> cmd;
    int rc=0;

    if (parseCmdLine(argc, argv)<0)
        exit(1);
    
    initStuff();
    
    bool lOut=false;
    while (!lOut) {
        cmd=getCommand();
        rc=processCommand(cmd);
        if (1 == rc) {
          lOut=true;
        }
    } // end while(!lOut)

    cout<<"That's all folks!"<<endl;
    return EXIT_SUCCESS;
}


static int processCommand(std::vector<std::string> &rCmd)
{
  // :fixme: :fixme: - try/catch for every commnad
        
  if (rCmd[0]=="about") {
      cout<<XFCAPP_NAME<<", version: "<<Xfc::getVersionString().c_str()<<endl;
  }
  else if (rCmd[0]=="help") {
    displayMessage("Commands:");
    displayMessage("  about");
    displayMessage("  exit | quit");
    displayMessage("  load <xmlFile>");
    displayMessage("  print [ all | disk <diskName> ]");
    displayMessage("  add <pathToFileOrDir> <diskName> [<maxDepth>]");
    displayMessage("  addWithoutSha <pathToFileOrDir> <diskName> [<maxDepth>]");
    displayMessage("    if path is a dir and ends with '/', the root dir is not added to xml tree");
    displayMessage("    depth= 0 - only root dir/file");
    displayMessage("    depth= 1 - root dir and one level below");
    displayMessage("    depth=-1 - infinite");
    displayMessage("  save [ <pathToFile> ]");
    displayMessage("  new [<catalogName>]");
    displayMessage("  force_new [<catalogName>]");
    displayMessage("  show [ disks ]");
    displayMessage("  find <text>");
    displayMessage("  label <diskOrFileName> <label>");
    displayMessage("    label can contain spaces");
    displayMessage("  description <diskOrFileName> <description>");
    displayMessage("    description can contain spaces");
    displayMessage("  details | display <path>");
    displayMessage("  setCDate <cdate>");
    displayMessage("    cdate = iso format: YYYY-MM-DD");
    displayMessage("  rename_disk <oldName> <newDisk>");
    displayMessage("  verify <diskName> <path>");
  }
  else if (rCmd[0]=="exit" || rCmd[0]=="quit") {
    return 1;
  }
  else if (rCmd[0]=="load") {            
    if (rCmd.size()==1) {
      displayError("load what? (load <xmlFile>)");
    }
    else {
      try {
        gXfc.loadFile(rCmd[1]);    
      }
      catch (std::string e) {
        displayError(e);
      }
    }
  }
  else if (rCmd[0]=="print") {
    if (rCmd.size()==1) {
      displayError("Print what? (print [ all | disk <diskName> ])");
    }
    else {
      if (rCmd[1]=="all")
        gXfc.parseFileTree(printToStdout, NULL);
      else if (rCmd[1]=="disk") {
        if (rCmd.size()<3) {
          displayError("Print what? (print [ all | disk <diskName> ])");
        }
        else {
          try {
            gXfc.parseDisk(printToStdout, rCmd[2]);
          }
          catch (std::string e) {
            displayError(e);
          }
        }
      }
    }
  }
  else if (rCmd[0]=="add" || rCmd[0]=="addWithoutSha") {
    if (rCmd.size()<3) {
      displayError("Add what? (add <fileOrDir> <diskName> [<maxDepth>])");
    }
    else {
      int lMaxDepth=-1;
      char *pPtr=NULL;
      bool lError=false;
      if (rCmd.size()>=4) {
        lMaxDepth=strtol(rCmd[3].c_str(), &pPtr, 10);
        if (lMaxDepth==0 && pPtr==rCmd[3].c_str()) { // error
          displayError("Invalid number (maxDepth)");
          lError=true;
        }
      }
      if (!lError) {
        try {
            vector<Xfc::XmlParamForFileCallback> cbList1;
            vector<Xfc::XmlParamForFileChunkCallback> cbList2;
            if (rCmd[0]=="add") {
                cbList1.push_back(sha256Callback);
                // params: path maxDepth abortFlag cbList1 cbList2 diskName ...
                gXfc.addPathToXmlTree(rCmd[1], lMaxDepth, NULL, cbList1, cbList2, rCmd[2]);
            }
            else
                gXfc.addPathToXmlTree(rCmd[1], lMaxDepth, NULL, cbList1, cbList2, rCmd[2]);
        }
        catch (std::string e) {
          displayError("Unexpected error: ", e);
        }
      }
    }
  }
  else if (rCmd[0]=="save") {
    std::string lName;
    if (rCmd.size()==1) { // no params
      displayError("Save where? (save <fileName>)");
    }
    else {
      char lAux[5];
      int i;
      if (fileExists(rCmd[1])) {
        for (i=0;i<999;i++) {
          sprintf(lAux, ".%03d", i);
          lName=rCmd[1]+lAux;
          if(!fileExists(lName))
            break;
        }
        if (i==1000) // wow, 1000 backups
          throw std::string("way too many backups");
        lName=rCmd[1]+lAux;
        rename(rCmd[1], lName);
      }
      lName=rCmd[1];
    }
    try {
      gXfc.saveToFile(lName, 1);
      displayMessage("File saved (filename=", lName, ")");
    }
    catch(std::string e) {
      displayError("Error saving file: ", e);
    }
  }
  else if (rCmd[0]=="new") {
    if (gXfc.getState()==0) {
      if (rCmd.size()==1)
        gXfc.createNew();
      else
        gXfc.createNew(rCmd[1]);
      displayMessage("OK");
    }
    else {
      displayError("There is a catalog in memory. Use force_new to create a new one");
    }
  }
  else if (rCmd[0]=="force_new") {
    if (rCmd.size()==1)
      gXfc.createNew();
    else
      gXfc.createNew(rCmd[1]);
    displayMessage("OK");
  }            
  else if (rCmd[0]=="show") {
    if (rCmd.size()<2) {
      displayError("Show what?");
    }
    else {
      if (rCmd[1]=="disks") {
        try {
          std::vector<std::string> lVect;
          lVect=gXfc.getDiskList();
          for (unsigned short i=0;i<lVect.size();i++) {
            std::string lDescription;
            lDescription=gXfc.getDescriptionOfNode(gXfc.getNodeForPath(lVect[i]));
            displayMessage(lVect[i], " - ", lDescription);
          }
        }
        catch(std::string e) {
          displayError("Error displaying disks");
          displayError(e);
        }
      }
    }
  }
  else if (rCmd[0]=="find") {
    if (rCmd.size()<2) {
      displayError("Find what?");
    }
    else {
      gXfc.parseFileTree(findInTree, (void*)rCmd[1].c_str());
      for (unsigned short i=0;i<gSearchResultsPaths.size();i++) {
        cout<<gSearchResultsPaths[i].c_str()<<endl;
        // :fixme: - show more details
      }
      gSearchResultsPaths.clear();
      gSearchResultsNodes.clear();
    }
  } 
  else if (rCmd[0]=="label") {
    if (rCmd.size()<3) {
      displayError("Label what?");
    }               
    else {
      std::string lLabel;
      lLabel=rCmd[2];
      for (unsigned int i=3;i<rCmd.size();i++) {
        lLabel+=" ";
        lLabel+=rCmd[i];
        // :fixme: - take real string
        // doesnt work if the command is "label disk one    two  three"
      }
      try {
        gXfc.addLabelTo(rCmd[1], lLabel);
      }
      catch (std::string e) {
        displayError("Error setting label for: ", rCmd[0]);
        displayError(e);
      }
    }
  }   
  else if (rCmd[0]=="description") {
    if (rCmd.size()<3) {
      displayError("Description of what?");
    }               
    else {
      std::string lDescription;
      lDescription=rCmd[2];
      for (unsigned int i=3;i<rCmd.size();i++) {
        lDescription+=" ";
        lDescription+=rCmd[i];
        // :fixme: - take real string
        // doesnt work if the command is "label disk one    two  three"
      }
      try {
        gXfc.setDescriptionOf(rCmd[1], lDescription);
      }
      catch (std::string e) {
        displayError("Error setting description for: ", rCmd[0]);
        displayError(e);
      }
    }
  }
  else if (rCmd[0]=="details" || rCmd[0]=="display") {
    if (rCmd.size()<2) {
      displayError("Details of what?");
    }     
    else {
      std::string lPath;
      lPath=rCmd[1];
      xmlNodePtr lpNode=gXfc.getNodeForPath(lPath);
      if(lpNode==NULL) {
        displayMessage("No xml node for ", lPath);
      }
      else {
        try {
            map<string, string> details;
            details=gXfc.getDetailsForNode(lpNode);
            displayMessage("Description: ", details["description"]);
            if (!details["cdate"].empty())
                displayMessage("Creation date: ", details["cdate"]);
            if (!details[SHA256LABEL].empty())
                displayMessage("sha256sum: ", details[SHA256LABEL]);
            if (!details[SHA1LABEL].empty())
                displayMessage("sha1sum: ", details[SHA1LABEL]);

            char labelsBuf[7]= { "labelX" };
            for (char i='0'; i<='9'; i++) {
                labelsBuf[5]=i;
                if (!details[labelsBuf].empty())
                    displayMessage("Label: ", details[labelsBuf]);
            }
        }
        catch (std::string e) {
          displayError("Error getting details for: ", lPath);
          displayError(e);
        }
      }
    }                
  }        
  else if (rCmd[0]=="setCDate") {
    if (rCmd.size()<3) {
      displayError("Usage: setCDate <diskName> <cdate> (<diskName> without '/')");
    }     
    else {
      std::string lPath;
      std::vector<std::string> lVect;
      lPath=rCmd[1];
                
      try {
        gXfc.setCDate(std::string("/")+lPath, rCmd[2]);
      }
      catch (std::string e) {
        displayError(std::string("Error setting details for: ")+lPath);
        displayError(e);
      }
    }                
  }
  else if (rCmd[0]=="rename_disk") {
    cmdRenameDisk(rCmd);
  }
  else if ("verify"==rCmd[0]) {
      if (rCmd.size()<3) {
          displayError("Usage: verify <diskName> <path>");
      }
      else {
          cmdVerifyDisk(rCmd);
      }
  }
  else {
    displayError("Unknown commnad");
  }

  return 0;
}


static void
displayDiff(string left, string middle, string right)
{
    displayMessage(" -* ", left);
    displayMessage("    ", right);
}


static void
cmdVerifyDisk(std::vector<std::string> cmd)
{
    vector<EntityDiff> differences;
    int rc;
    rc=gXfc.verifyDirectory(cmd[1], cmd[2], &differences, NULL);

    // results
    if(2 == rc)
        displayMessage("Some (or all) files did not have checksums.");

    if (differences.size()>0) {
        string str1, str2;
        for (unsigned int i=0; i<differences.size(); i++) {
            switch (differences[i].type) {
            case eDiffOnlyInCatalog:
                displayDiff(differences[i].name, "!", "-");
                break;
            case eDiffOnlyOnDisk:
                displayDiff("-", "!", differences[i].name);
                break;
            case eDiffSize:
                str1=differences[i].name+" (catalog size: "+differences[i].catalogValue+")";
                str2=differences[i].name+" (disk size: "+differences[i].diskValue+")";
                displayDiff(str1, " ! ", str2);
                str1.clear();
                str2.clear();
                break;
            case eDiffSha256Sum:
                str1=differences[i].name+" (catalog sha256: "+differences[i].catalogValue+")";
                str2=differences[i].name+" (disk sha256: "+differences[i].diskValue+")";
                displayDiff(str1, " ! ", str2);
                str1.clear();
                str2.clear();
                break;
            case eDiffSha1Sum:
                str1=differences[i].name+" (catalog sha1: "+differences[i].catalogValue+")";
                str2=differences[i].name+" (disk sha1: "+differences[i].diskValue+")";
                displayDiff(str1, " ! ", str2);
                str1.clear();
                str2.clear();
                break;
            case eDiffErrorOnDisk:
                str1=differences[i].name;
                str2=differences[i].name+" - error reading details from disk: "+differences[i].diskValue;
                displayDiff(str1, " ! ", str2);
                str1.clear();
                str2.clear();
                break;
            default:
                displayDiff(differences[i].name, " ! ", differences[i].name);
                break;
            }                
        }
    }
    else {
        displayMessage("Disk OK");
    }

}


static void cmdRenameDisk(std::vector<std::string> cmd)
{
  std::string diskPath;
  xmlNodePtr xmlNode;
  
  if (cmd.size()==2) {
    displayError("rename what? (rename_disk <oldDiskName> <newDiskName>)");
  }
  else {
    try {
      displayMessage(std::string(cmd[1])+" -> "+cmd[2]);
      diskPath=std::string("/")+cmd[1];
      if ( (xmlNode = gXfc.getNodeForPath(diskPath) ) == NULL) {
        displayError("No such disk");
      }
      else {
        gXfc.setNameOfElement(xmlNode, cmd[2]);
      }
    }
    catch (std::string e) {
      displayError(e);
    }
  }

}


int printToStdout(unsigned int lDepth, std::string lPath, Xfc& lrXfc, xmlNodePtr lpNode,
                  void *lpParam)
{
    for(unsigned short i=0;i<lDepth; i++)
        cout<<" ";
    cout<<(const char*)lpNode->name<<":"<<lrXfc.getNameOfElement(lpNode).c_str()<<endl;
    return 0;
}  


int findInTree(unsigned int lDepth, std::string lPath, Xfc& lrXfc, xmlNodePtr lpNode,
               void *lpParam)
{
    std::string lName;
    const char *lpPtr=(const char*)lpParam;
    lName=lrXfc.getNameOfElement(lpNode); // :fixme: utf8 -> string ?
    if(xmlStrcasestr((const xmlChar*)lName.c_str(),
       (xmlChar*)lpPtr)!=NULL)
    {
#if defined(XFC_DEBUG)
        cout<<":debug: found matching node: path="<<lPath.c_str();
        cout<<", name="<<lName.c_str()<<endl;
#endif 
        gSearchResultsPaths.push_back(lPath+"/"+lName);
        gSearchResultsNodes.push_back(lpNode);
    }
    // :fixme: - check labes and other stuff
    
    return 0;
}

void initStuff()
{
    xmlIndentTreeOutput=1;
    xmlKeepBlanksDefault(0);
    gLog.setVerboseLevel(3);
}
