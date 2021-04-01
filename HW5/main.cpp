#include <iostream>
#include "dirent.h"
#include <string.h>
#include <string>
#include <vector>
#include <direct.h>
#include <fstream>
#include "Vector.h"
#include "md5.h"
using namespace std;

void list_files(const string& dir_path)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dir_path.c_str())) != NULL)
    {
      //print all the files and directories within directory
      while ((ent = readdir (dir)) != NULL)
      {
          if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
          {
              continue;
          }
          printf ("%s%s\n", dir_path.c_str(), ent->d_name);

          if(ent->d_type == DT_DIR)
          {
              list_files(dir_path + std::string(ent->d_name) + "\\");
          }
      }
      closedir (dir);
    }
}


void copyFile(const string& source, const string& destination)
{
    ifstream  src(source, ios::binary);
    ofstream  dst(destination, ios::binary);
    dst << src.rdbuf();
}


void remove_file(const string& file_name)
{
    if(remove(file_name.c_str()) != 0)
    {
        cout << "Can't remove" << file_name <<" \n";
    }
    else
    {
        cout << file_name << " removed successfully\n";
    }
}


void remove_dir(const string& dir_name)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (dir_name.c_str())) != NULL)
    {
      while ((ent = readdir (dir)) != NULL)
      {
          if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
          {
              continue;
          }

          if(ent->d_type == DT_DIR)
          {
              remove_file(dir_name + string(ent->d_name) + "\\");
          }
          remove_file(dir_name + string(ent->d_name));
      }
      remove_file(dir_name);
      closedir (dir);
    }
}




bool is_dir(string path)
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL)
    {
        ent = readdir(dir);
        if(ent -> d_type == DT_DIR)
        {
            return true;
        }
        else
            false;
    }
    closedir (dir);
}



void copyDir(string inputDir, string outDir)
{
    DIR *cDIR;
    struct dirent *ent;
    string tmp, tmpPath, outPath, inputDirStr = inputDir;

    if (is_dir(inputDir) == false)
    {
        cout << "This is not a directory" << endl;
        return;
    }

    if((cDIR = opendir(inputDirStr.c_str())) != NULL)
    {

        while((ent = readdir(cDIR)) != NULL)
        {
            tmp = ent -> d_name;
            if(strcmp(ent -> d_name, ".") != 0 && strcmp(ent -> d_name, "..") != 0 )
            {
                tmpPath = inputDirStr;
                tmpPath.append("\\");
                tmpPath.append( tmp );
                cout << ent -> d_name;
                if(is_dir(tmpPath.c_str()))
                {
                    cout << "-> folder" << endl;
                    outPath = outDir;
                    outPath.append("\\");
                    outPath.append(tmp);
                    _mkdir(outPath.c_str());
                    copyDir(tmpPath.c_str(), outPath);
                }
                else
                {
                    cout << "-> file"   << "\n";
                    outPath = outDir;
                    outPath.append("\\");
                    outPath.append(tmp);
                    copyFile(tmpPath.c_str(), outPath.c_str());
                }
            }
        }
        closedir(cDIR);
    }
}


string hash_of_file(const string& file_name)
{
    fstream bin(file_name, fstream::in | fstream::binary | fstream::ate);
    if(bin.is_open() == false)
    {
        cout << "Can't open " << file_name << endl;
        return "";
    }

    size_t len = bin.tellg();
    bin.seekg(0);
    unsigned char* buffer = new unsigned char[len];
    bin.read((char*)buffer, len);
    MD5 hasher;
    hasher.update(buffer, len);
    hasher.finalize();
    delete[] buffer;

    return hasher.hexdigest();
}



string hash_of_string(const string& file_name)
{
    return md5(file_name);
}


void synchronization(string inputDir, string outputDir, int max)
{
    DIR *aDir;
    DIR *bDir;
    struct dirent *aEnt, *aEntNext;
    struct dirent *bEnt, *bEntNext;
    string tmpA, tmpB, tmpPathA, tmpPathB, outPath, tmpAnext, tmpBnext, inputDirStr = inputDir, outputDirStr = inputDirStr;
    int i = 0;

    if(is_dir(inputDir) == false)
    {
        cout << "This is not a directory" << endl;
    }

    if((aDir = opendir(inputDir.c_str())) != NULL)
    {
        if((bDir = opendir(outputDir.c_str())) != NULL)
        {
            do // get folders and files names
            {
                aEnt = readdir(aDir);
                tmpA = aEnt -> d_name;
                aEntNext = readdir(aDir);
                tmpAnext = aEntNext -> d_name;

                bEnt = readdir(bDir);
                tmpB = bEnt -> d_name;
                bEnt = readdir(bDir);
                tmpBnext = bEntNext -> d_name;


                if((strcmp(aEnt -> d_name, ".") != 0 && strcmp(aEnt -> d_name, "..") != 0) &&
                   (strcmp(bEnt -> d_name, ".") != 0 && strcmp(bEnt -> d_name, "..") != 0))
                {
                    tmpPathA = inputDir;
                    tmpPathA.append("\\");
                    tmpPathA.append(tmpA);

                    tmpPathB = outputDir;
                    tmpPathB.append("\\");
                    tmpPathB.append(tmpB);

                    if(hash_of_string(tmpA) == hash_of_string(tmpB))
                    {
                        i++;
                    }
                    else
                    {
                        //variant 1 - da ima dobaven nov/a fail/direktoriq v Activnata
                        //ako hesha i+1 ot aktivnata e = na hesha na i ot backup => dobavqm faila/direktoriq v backup

                        //variant 2 - da ima iztrit/a fail/direktoriq ot Activnata
                        //ako hesha na i ot aktivnata e = na hesha na i+1 ot backup => premahvam elementa ot backup

                        //variant 3 - promenen fail

                        if(hash_of_string (tmpAnext) == hash_of_string(tmpB))
                        {
                            if(is_dir(inputDirStr.c_str()))
                            {
                                cout << "-> folder" << endl;
                                outPath = outputDirStr;
                                outPath.append("\\");
                                outPath.append(tmpAnext);
                                _mkdir(outPath.c_str());
                                synchronization(inputDir.c_str(), outPath.c_str(), max);
                            }
                            else
                            {
                                cout << "-> file"   << "\n";
                                outPath = outputDirStr;
                                outPath.append("\\");
                                outPath.append(tmpAnext);
                                synchronization(inputDir.c_str(), outPath.c_str(), max);
                            }
                        }
                        else if(hash_of_string(tmpAnext) == hash_of_string(tmpBnext))
                        {
                            if(is_dir(inputDirStr.c_str()))
                            {
                                cout << "-> folder" << endl;
                                remove_dir(tmpB);
                                copyDir(tmpA, tmpB);
                            }
                            else
                            {
                                remove_file(tmpB);
                                copyFile(tmpA, tmpB);
                            }
                        }
                        else if(hash_of_string(tmpA) == hash_of_string(tmpBnext))
                        {
                            if(is_dir(inputDirStr.c_str()))
                            {
                                remove_dir(tmpBnext);
                            }
                            else
                            {
                                remove_file(tmpBnext);
                            }
                        }

                    }
                }
        }
        while(i < max);
        closedir(bDir);
        }
    closedir(aDir);
    }
}


void toVector(DIR *dir, struct dirent *ent, Vector<string> dirlist, string dir_name)
{
    if ((dir = opendir(dir_name.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if(ent -> d_name == "." || ent -> d_name == "..")
            {
                continue;
            }
            if(ent -> d_type == DT_DIR)
            {
                toVector(dir, ent, dirlist, dir_name + string(ent -> d_name) + "\\");
            }
            toVector(dir, ent, dirlist, dir_name + string(ent -> d_name));
        }
        dirlist.Push(dir_name);
        closedir(dir);
    }
}







int main(int argc,char* argv[])
{
    if(argc == 3)
    {
        //Active
        DIR *dirActive;
        struct dirent *entActive;
        Vector<string> dirlistActive;
        string d_nameActive = argv[1];

        toVector(dirActive, entActive, dirlistActive, d_nameActive);


        //Backup
        DIR *dirBackup;
        struct dirent *entBackup;
        Vector<string> dirlistBackup;
        string d_nameBackup = argv[2];

        toVector(dirBackup, entBackup, dirlistActive, d_nameBackup);

        int max = dirlistActive.Size() >= dirlistBackup.Size() ? dirlistActive.Size() : dirlistBackup.Size();


        //empty backup
        if(dirlistBackup.Size() == 0 && dirlistActive.Size() > 0)
        {
            copyDir(d_nameActive, d_nameBackup);
        }
        else if(dirlistBackup.Size() > 0 && dirlistActive.Size() > 0)
        {
            synchronization(d_nameActive, d_nameBackup, max);
        }
        else
        {
            cout << "empty directory" << endl;
        }

        cout << "active" << endl;
        list_files(d_nameActive);
        cout << "backup" << endl;
        list_files(d_nameBackup);
    }


    else if(argc == 4)
    {
        DIR *dirActive;
        struct dirent *entActive;
        Vector<string> dirlistActive;
        string d_nameActive = argv[2];

        toVector(dirActive, entActive, dirlistActive, d_nameActive);


        //Backup
        DIR *dirBackup;
        struct dirent *entBackup;
        Vector<string> dirlistBackup;
        string d_nameBackup = argv[3];

        toVector(dirBackup, entBackup, dirlistBackup, d_nameBackup);

        int max = dirlistActive.Size() >= dirlistBackup.Size() ? dirlistActive.Size() : dirlistBackup.Size();

        //empty active
        if(dirlistActive.Size() == 0 && dirlistBackup.Size() > 0)
        {
            copyDir(d_nameBackup, d_nameActive);
        }
        else if(dirlistBackup.Size() > 0 && dirlistActive.Size() > 0)
        {
            synchronization(d_nameBackup, d_nameActive, max);
        }
        else
        {
            cout << "empty directory" << endl;
        }


        cout << "active" << endl;
        list_files(d_nameActive);
        cout << "backup" << endl;
        list_files(d_nameBackup);
    }


    else
    {
        cout << "Expected 3(backup.exe <active_directory> <backup_recovery_copy>) or "<< endl;
        cout << "4(backup.exe [-restore] <active_directory> <backup_resource_copy>) arguments, but given " << argc << endl;
    }

    return 0;
}
