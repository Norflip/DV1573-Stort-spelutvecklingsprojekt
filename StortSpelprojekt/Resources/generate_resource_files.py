import glob
import os
from shutil import copyfile

dirname = os.path.dirname(__file__)

texture_folder = "..\\\\StortSpelprojekt\\\\Textures"
shader_folder = "..\\\\StortSpelprojekt\\\\Shaders"
sound_folder = "..\\\\StortSpelprojekt\\\\Sounds"
relative_debug_folder = "..\\\\Debug"
relative_release_folder = "..\\\\Release"
data_folder = "..\\\\StortSpelprojekt\\\\data"

rc_map = "resourceMap.txt"
baseIndex = 100
currentIndex = 1

header      = open(os.path.join(dirname,                "resource.h"),      "w+")
resource    = open(os.path.join(dirname,                "Resource.rc"),     "w+")
rsmap       = open(os.path.join(dirname, data_folder,   "resourceMap.txt"), "w+")
resource.write("#include \"resource.h\"\n")

classNames = ["TXT", "SHADER", "TEXTURE", "SOUND"]
classIndex = 51

header.write("#pragma once\n")

for i in (classNames) :
    header.write("#define " + i + " " + str(classIndex) + "\n")
    classIndex += 1

txtfiles = ["resourceMap", "Resources_objects", "Resources_shaders", "Resources_textures"]
txtNameIndex = 1

for i in (txtfiles) :
    key = "IDR_TXT" + str(txtNameIndex)
    
    header.write("#define IDR_TEXT_" + i.upper() + " " + str(baseIndex + txtNameIndex) + "\n")
    resource.write("IDR_TEXT_" + i.upper() + " TXT \"" + os.path.join(dirname, data_folder) + "\\\\" + i + ".txt\"\n")
    currentIndex += 1
    txtNameIndex += 1


def add (mKeyPrefix, mClass, mFolder, mFileType, mIndex):
    currentIndex = int(mIndex)
    localIndex = 1
    for i in glob.glob(mFolder + "\\*." + mFileType) :
        fileName = i.rsplit('\\', 1)[1]
        key = mKeyPrefix + str(localIndex)

        rsmap.write(fileName + " : " + str(baseIndex + currentIndex) + "\n")
        resource.write(key + " " + mClass + " \"" + mFolder + "\\\\" + fileName + "\"\n")
        header.write("#define " + key + " " + str(baseIndex + currentIndex) + "\n")
        currentIndex += 1
        localIndex += 1
    return currentIndex

#rsmap.write("shaders:\n")
currentIndex = add("IDR_SHADER", "SHADER", shader_folder, "*", currentIndex)

#rsmap.write("textures:\n")
currentIndex = add("IDR_TEXTURE", "TEXTURE", texture_folder, "*", currentIndex)

#rsmap.write("sounds:\n")
currentIndex = add("IDR_SOUND", "SOUND", sound_folder, "*", currentIndex)

header.write("// Next default values for new objects\n")
header.write("#ifdef APSTUDIO_INVOKED\n")
header.write("#ifndef APSTUDIO_READONLY_SYMBOLS\n")
header.write("#define _APS_NEXT_RESOURCE_VALUE        " + str(currentIndex) + "\n")
header.write("#define _APS_NEXT_COMMAND_VALUE         40001\n")
header.write("#define _APS_NEXT_CONTROL_VALUE         1001\n")
header.write("#define _APS_NEXT_SYMED_VALUE           "+ str(baseIndex + 1) + "\n")
header.write("#endif\n")
header.write("#endif\n")

header.close()
resource.close()
rsmap.close()

copyfile(os.path.join(dirname, "resource.h"), "..\\StortSpelprojekt\\ResourceKeys.h")

input("Press Enter to exit.")