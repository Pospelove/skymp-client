from os import listdir
from os.path import isfile, join

print "CostileDragon CodeGen Started"

sTypeVoid = "void" # const

# get all files in directory
sDir = "in/"
onlyFiles = [sFileName for sFileName in listdir(sDir) if isfile(join(sDir, sFileName))]
print onlyFiles

fOut = open("out_papyrus.txt", "w");
fOutCpp = open("out_cpp.txt", "w");
fOutHpp = open("out_hpp.txt", "w");

for sScriptName in onlyFiles:
    
    sClass = sScriptName[0:sScriptName.find(".")]
    print sScriptName + ": Starting code generation"

    print "Creating Papyrus code ... "

    fOutThis = open("out/Costile2_" + sClass + ".psc", "w");
    fIn = open("in/" + sScriptName)

    # print Script Start
    fOutThis.write("Scriptname Costile2_" + sClass + " Hidden\n\n")

    # print Papyrus comment
    sComment = (";\n" * 3) + "; script " + sScriptName + "\n" + (";\n" * 3) + "\n"
    fOut.write(sComment)
    fOutThis.write(sComment)
    # (something like this)
    """
    ;
    ;
    ;
    ; script %scriptName%
    ;
    ;
    ;
    """

    # print C++ comment
    sComment = "// " + sScriptName + "\n\n"
    fOutCpp.write(sComment)


    bPropertyBody = False
    
    for sLine in fIn:
        
        # ignore wrong lines (not function declaration)
        if sLine.isspace() or sLine[0] == ';':
            continue
        if sLine.lower().find("function") == -1:
            continue
        if sLine.lower().find("endfunction") != -1:
            continue
        if sLine.find("[]") != -1:
            print "'" + sName + "(...)':\n\t Warning - Arrays aren't supported, skipping"
            continue

        # ignore properties
        bFoundProperty = sLine.lower().find("property") != -1
        if bFoundProperty:
            bPropertyBody = not bPropertyBody
        if bPropertyBody:
            continue;

        # get function flags
        bNative = sLine.lower().rfind("native") > sLine.lower().rfind(")")
        bGlobal = sLine.lower().rfind("global") > sLine.lower().rfind(")")

        lineParts = sLine.split(' ');

        # get function type
        sType = lineParts[0].lower()
        if sType == "function": # type not found
            sType = sTypeVoid
            lineParts.insert(0, sType)

        # get function name
        sName = lineParts[2]
        iPos = sName.find("(")
        sName = sName[0:iPos]

        # get argument types and names
        argumentTypes = []
        argumentNames = []
        iArgumentCount = 0
        sArguments = sLine[sLine.find("(") + 1 : sLine.find(")")]
        for sArgument in sArguments.split(","):
            argumentPartsRaw = sArgument.split(" ")
            argumentParts = []
            bIgnoreNextElement = False
            for s in argumentPartsRaw:
                if s == "" or s.isspace():
                    continue;
                if s.find("=") != -1 and len(s) != 1: # thanks to bethesda for 'aiExampleArgument=defaultVal' without space
                    continue;
                if s == "=":
                    bIgnoreNextElement = True
                    continue;
                if bIgnoreNextElement:
                    bIgnoreNextElement = False;
                    continue;
                argumentParts.append(s)
            bNextTokenIsTypename = True
            for s in argumentParts:
                if bNextTokenIsTypename:
                    argumentTypes.append(s.lower())
                else:
                    argumentNames.append(s)
                bNextTokenIsTypename = not bNextTokenIsTypename

        if len(argumentTypes) != len(argumentNames):
            print "'" + sName + "(...)':\n\t Warning - len(argumentTypes) != len(argumentNames) [" + str(len(argumentTypes)) + ";" + str(len(argumentNames)) + "]" 
            
        iArgumentCount = min([len(argumentTypes), len(argumentNames)])

        # print comment
        sComment = "; " + sType + " " + sClass + "::" + sName + "(...)"
        if bNative:
            sComment += " native"
        if bGlobal:
            sComment += " global"
        sComment += "\n"
        fOut.write(sComment)
        fOutThis.write(sComment)


        # generate Papyrus code
    
        # 1) type
        if sType != sTypeVoid:
            sText = sType + " "
        else:
            sText = ""

        # 2) name
        sText += "function " + sClass + "_" + sName
        sText += "() global\n"

        # 3) body: read arguments

        def GetReadArgumentExpression(sType, iIndex):
            sExpression = "Costile2.Get"
            bSimpleType = sType == "int" or sType == "float" or sType == "bool" or sType == "string"
            if bSimpleType:
                sExpression += sType;
                sExpression = sExpression[:12] + sExpression[12].upper() + sExpression[13:]
            else:
                sExpression += "TESForm"
            sExpression += "(session, " + str(iIndex) + ")"
            if not bSimpleType:
                #sExpression = "Game.GetForm(" + sExpression + ")"
                sExpression += " as " + sType
            return sExpression

        def GetArgumentPrefix():
            return "cdcg_"

        sText += '\tint session = Costile2.GetSession("' + sClass + "_" + sName + '")\n'
    
        myRange = range(iArgumentCount);
        iOffset = 0
        if not bGlobal:
            iOffset = 1
            sText += "\t" + sClass + " this = " + GetReadArgumentExpression(sClass, 0) + "\n"
        
        for i in myRange:
            sText += "\t" + argumentTypes[i] + " " + GetArgumentPrefix() + argumentNames[i] + " = " + GetReadArgumentExpression(argumentTypes[i], i + iOffset) + "\n"
    
        # 4) body: execute
        sText += "\t"
        if sType != sTypeVoid:
            sText += "return "
        if bGlobal:
            sText += sClass + "." + sName
        else:
            sText += "this." + sName
        sText += "("
        for i in myRange:
            sText += GetArgumentPrefix() + argumentNames[i] + ", "
        if len(myRange) > 0:
            sText = sText[:-2]
        sText += ")\n"
    
        # 5) end function
        sText += "endFunction\n\n"

        # write result
        fOut.write(sText)
        fOutThis.write(sText)
    
        # generate C++ code
    
        def PapyrusTypenameToCppTypename(sType):
            sTypesDict = {"int": "SInt32","bool":"bool","float":"float","string":"std::string","objectreference":"Value<TESObjectREFR>","actor":"Value<Actor>","objectreference":"Value<TESObjectREFR>", "keyword":"Value<BGSKeyword>","textureset":"Value<BGSTextureSet>",
                "globalvariable":"Value<TESGlobal>","class":"Value<TESClass>","faction":"Value<TESFaction>","headpart":"Value<BGSHeadPart>","eyes":"Value<TESEyes>","race":"Value<TESRace>",
                "sound":"Value<TESSound>","acousticspace":"Value<BGSAcousticSpace>","enchantment":"Value<EnchantmentItem>","spell":"Value<SpellItem>","scroll":"Value<ScrollItem>","activator":"Value<TESObjectACTI>",
                "ralkingactivator":"Value<BGSTalkingActivator>","armor":"Value<TESObjectARMO>","book":"Value<TESObjectBOOK>","container":"Value<TESObjectCONT>","door":"Value<TESObjectDOOR>","ingredient":"Value<IngredientItem>",
                "light":"Value<TESObjectLIGH>","misc":"Value<TESObjectMISC>","apparatus":"Value<BGSApparatus>","static":"Value<TESObjectSTAT>","staticCollection":"Value<BGSStaticCollection>","movablestatic":"Value<BGSMovableStatic>",
                "grass":"Value<TESGrass>","treeobject":"Value<TESObjectTREE>","flora":"Value<TESFlora>","furniture":"Value<TESFurniture>","weapon":"Value<TESObjectWEAP>","ammo":"Value<TESAmmo>",
                "actorbase":"Value<TESNPC>","leveledcharacter":"Value<TESLevCharacter>","key":"Value<TESKey>","potion":"Value<AlchemyItem>","idlemarker":"Value<BGSIdleMarker>","constructibleobject":"Value<BGSConstructibleObject>",
                "projectile":"Value<BGSProjectile>","hazard":"Value<BGSHazard>","soulgem":"Value<TESSoulGem>","leveleditem":"Value<TESLevItem>","weather":"Value<TESWeather>","climate":"Value<TESClimate>",
                "cell":"Value<TESObjectCELL>","worldspace":"Value<TESWorldSpace>","topic":"Value<TESTopic>","topicinfo":"Value<TESTopicInfo>","quest":"Value<TESQuest>","idle":"Value<TESIdleForm>",
                "package":"Value<TESPackage>","combatstyle":"Value<TESCombatStyle>","loadscreen":"Value<TESLoadScreen>","leveledspell":"Value<TESLevSpell>","imagespacemodifier":"Value<TESImageSpaceModifier>",
                "formlist":"Value<BGSListForm>","perk":"Value<BGSPerk>","bodypartdata":"Value<BGSBodyPartData>","voicetype":"Value<BGSVoiceType>","encounterzone":"Value<BGSEncounterZone>","location":"Value<BGSLocation>",
                "message":"Value<BGSMessage>","wordofpower":"Value<TESWordOfPower>","shout":"Value<TESShout>","scene":"Value<BGSScene>","outfit":"Value<BGSOutfit>","art":"Value<BGSArtObject>","material":"Value<BGSMaterialObject>",
                "sounddescriptor":"Value<BGSSoundDescriptorForm>","activemagiceffect":"Value<ActiveMagicEffect>","magiceffect":"Value<EffectSetting>","associationtype":"Value<BGSAssociationType>",
                "colorform" : "Value<BGSColorForm>", "actorvalueinfo" : "Value<ActorValueInfo>", "armoraddon" : "Value<TESObjectARMA>", "equipslot" : "Value<BGSEquipSlot>", "leveledactor" : "Value<TESLevCharacter>",
                "locationreftype" : "Value<BGSLocationRefType>", "effectshader" : "Value<TESEffectShader>", "explosion" : "Value<BGSExplosion>", "impactdataset" : "Value<BGSImpactDataSet>", "musictype" : "Value<BGSMusicType>",
                "form": "Value<TESForm>", "void" : "void" }
            try:
                return sTypesDict[sType.lower()]
            except KeyError:
                print "'" + sName + "(...)':\n\t Warning - Unknown Papyrus Type '" + sType.lower() + "'"
                return "Value<TESForm>"

        # 1) macro name
        sMacro1 = "DEFVAL_START"
        sMacro2 = "DEFVAL_END"
        if sType == sTypeVoid:
            sMacro1 = "DEFVOID_START"
            sMacro2 = "DEFVOID_END"
        if iArgumentCount == 0 and bGlobal:
            sMacro1 += "_ZEROARGS"
            sMacro2 += "" # doesn't need "_ZEROARGS"

        # 2) macro arguments
        sMacro1 += "("
        sMacro2 += "("
        
        if sType != sTypeVoid:
            sMacro1 += PapyrusTypenameToCppTypename(sType) + ", "
    
        sMacro1 += sClass + ", " + sName + ", "
            
        myRange = range(iArgumentCount)
        if not bGlobal:
            sMacro1 += PapyrusTypenameToCppTypename(sClass) + " " + "self" + ", "
            sMacro2 += "self" + ", "
        for i in myRange:
            sMacro1 += PapyrusTypenameToCppTypename(argumentTypes[i]) + " " + argumentNames[i] + ", "
            sMacro2 += argumentNames[i] + ", "
    
        if iArgumentCount == 0 and bGlobal:
            sMacro2 += "NULL" + ", "
            
        sMacro1 = sMacro1[:-2] + ");"
        sMacro2 = sMacro2[:-2] + ");"

        # write result
        sText = sMacro1 + "\n" + sMacro2 + "\n\n"
        fOutCpp.write(sText)

        # generate C++ header

        sText = PapyrusTypenameToCppTypename(sType) + " " + sName + "("
        if iArgumentCount != 0 or not bGlobal:
            sText += sMacro1[sMacro1.rfind(sName) + 2 + len(sName) : sMacro1.rfind(")")]
            sText += ", "
        sCallbackArgument = "std::function<void(" + PapyrusTypenameToCppTypename(sType) + ")> callback = {}"
        sText += sCallbackArgument
        sText += ");\n"
        fOutHpp.write(sText)
        
        
    fIn.close()
    fOutThis.close()
    
fOut.close()
fOutCpp.close()
fOutHpp.close()
print "Done"
