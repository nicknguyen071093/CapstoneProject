#include "Showcontent.h"

ShowContent::ShowContent()
{
}
QString ShowContent::concatenateString(QString addLetter, QString str){
    int x = addLetter.compare("MU", Qt::CaseInsensitive);
    int y = addLetter.compare("RAU", Qt::CaseInsensitive);
    int i = str.size() -1 ;
    if(x==0){

        if(str.endsWith("a")){
            str = str.replace(i,1,"â");
        }else if(str.endsWith("e")){
            str = str.replace(i,1,"ê");
        }else if(str.endsWith("o")){
            str = str.replace(i,1,"ô");
        }

    }else if(y==0){
        if(str.endsWith("o")){
            str = str.replace(i,1,"ơ");
        }
    }else{
        str= str.append(addLetter);
    }
    return str;
}
void ShowContent::speechString(QString s){
    QByteArray ba = s.toLatin1();
     const char *str = ba.data();
    espeak_POSITION_TYPE position_type;
    espeak_AUDIO_OUTPUT output;
    char *path=NULL;
    int Buflength = 500, Options=0;
    void* user_data;
    t_espeak_callback *SynthCallback;
    espeak_PARAMETER Parm;
    void speak(const char * str);
    char Voice[] = {"vietnam"};
    unsigned int Size,position=0, end_position=0, flags=espeakCHARS_AUTO, *unique_identifier;
    output = AUDIO_OUTPUT_PLAYBACK;
    int I, Run = 1, L;
    espeak_Initialize(output, Buflength, path, Options );
    espeak_SetParameter(espeakVOLUME,200,0);
    espeak_SetParameter(espeakRATE,140,0);
    espeak_SetParameter(espeakPITCH,75,0);
    espeak_SetParameter(espeakRANGE,50,0);
    espeak_VOICE *spec = (espeak_VOICE*) malloc(sizeof(espeak_VOICE));
      spec->name = "vie";
      spec->languages = "vi";
      spec->identifier = "vi";
      spec->gender = 1;
      spec->age =20;
      spec->variant = 0;
      espeak_SetVoiceByProperties(spec);
    //espeak_SetVoiceByName(Voice);
    Size = strlen(str)+1;

    espeak_Synth( str, Size, position, position_type, end_position, flags,
    unique_identifier, user_data );
    espeak_Synchronize( );

}
