#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Ping.h>

#define ssid "nauta_HogarB3FCF0"
#define password "72446959"
#define url1 "http://192.168.1.1/Forms/login_security_1"
#define url2 "http://192.168.1.1/status/status_deviceinfo.htm"
#define url3 "www.google.com"
#define T_Limt_Seg 100



unsigned long Time_T;
unsigned long Time_ST;
bool Time_Bool=false;
HTTPClient http,http1;


///////////////////////Funciones/////////////////

void ApagarRouter()
{
  /////////////////
}


int ConectarAlRouter()
{
  Serial.print("Conectando con el Router\n");
  //Inicio conexion
  http.begin(url1);
  //Agrego cabeceras
  http.addHeader("Host","192.168.1.1");
  http.addHeader("Connection","keep-alive");
  http.addHeader("Content-Length","196");
  http.addHeader("Cache-Control","max-age=0");
  http.addHeader("Upgrade-Insecure-Requests","1");
  http.addHeader("Origin","http://192.168.1.1");
  http.addHeader("Content-Type","application/x-www-form-urlencoded");
  http.addHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36");
  http.addHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
  http.addHeader("Referer","http://192.168.1.1/login_security.html");
  http.addHeader("Accept-Encoding","gzip, deflate");
  http.addHeader("Accept-Language","es-ES,es;q=0.9");
  http.addHeader("Cookie","C0=%00; C1=%00");
  //Agrego Cuerpo del Form
  String FromData="tipsFlag=0&timevalue=0&Login_Name=admin&Login_Pwd=Ha2S%2BeOKqmzA6nrlmTeh7w%3D%3D&uiWebLoginhiddenUsername=21232f297a57a5a743894a0e4a801fc3&uiWebLoginhiddenPassword=21232f297a57a5a743894a0e4a801fc3";
  //Solicito cabeceras de la respuesta
  const char * headerKeys[] = {"Location", "Set-Cookie"} ;
  http.collectHeaders(headerKeys,2);
  //Envio del Form
  int n=http.POST(FromData);
  //Libero el http
  http.end();
  
  Serial.print("Codigo Respuesta:\n");
  Serial.print(n);
  Serial.print("\n"); 
  
  return n;  
}
void ProcesarRespuesta()
{
    String response=http1.getString();
    //Buscar la cantidad de clientes conectados
    String s=response.substring(response.indexOf("WirelessClientNum.innerHTML=")+29);
    s=s.substring(0,s.indexOf(';'));
    char char_array[s.length()+1];
    s.toCharArray(char_array,s.length()+1);
    unsigned int contClientes=(unsigned int)atoi(char_array);
    
    Serial.print("Clientes:\n"); 
    Serial.print(contClientes); 
    Serial.print("\n"); 
    
    bool HayInternetBool=HayIinternet();
    if(contClientes==2 && HayInternetBool==true)
    {
      if(Time_Bool)
      {
        unsigned long time_t=millis();
        Time_ST+=(time_t-Time_T)/1000;
        Time_T=time_t;
        if(Time_ST>=T_Limt_Seg_Seg)
        {
          Time_ST=0;
          Serial.print("Apagando Router\n");
          ApagarRouter();
        }
      }
      else
      {
        Time_T=millis();
        Time_ST=0;
        Time_Bool=true;
      }
    }
    else
    {
      if(Time_Bool==true)
          Time_Bool==false; 
          
      //Leer las MAC conectadas
      /*
      s=response.substring(response.indexOf("MAC<"));
      Serial.print("MACs:\n");
      
      for(unsigned int i=0;i<contClientes;i++)
      {
        unsigned int unsig=s.indexOf("</td><td align=\"center\" class=\"tabdata\">");
        String MAC=s.substring(unsig+40,unsig+57);
        Serial.print(MAC);
        Serial.print("\n");
        unsig=s.indexOf("</td><td align=\"center\" class=\"tabdata\">");
        s=s.substring(unsig+57);
      }
      */
    }
}


int SolicitarDatos(String headerSet_Cookie)
{
  Serial.print("Solicitando Datos...\n");
  //Inicio la conexion
  http1.begin(url2);
  //Agrego cabeceras
  http1.addHeader("Host","192.168.1.1"); 
  http1.addHeader("Connection","keep-alive");
  http1.addHeader("Cache-Control","max-age=0");
  http1.addHeader("Upgrade-Insecure-Requests","1");
  http1.addHeader("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
  http1.addHeader("Referer","http://192.168.1.1/status/status_deviceinfo.htm");
  http1.addHeader("Accept-Encoding","gzip, deflate");
  http1.addHeader("Accept-Language","es-ES,es;q=0.9");
  //Agrego cabeceras de COOKIE
  String cookie="C0=";
  cookie+=headerSet_Cookie.substring(headerSet_Cookie.indexOf('=')+1,headerSet_Cookie.indexOf(';'));
  cookie+="; C1=";
  cookie+=headerSet_Cookie.substring(headerSet_Cookie.indexOf('=')+1,headerSet_Cookie.indexOf(';'));
 
  http1.addHeader("Cookie",cookie);
  //Envio el GET
  int n=http1.GET();
  //Libero el http
  http1.end();
  
  Serial.print("Codigo Respuesta:\n");
  Serial.print(n);
  Serial.print("\n");
  
  return n;
}
void Conectar_Wifi()
{

  WiFi.begin(ssid,password);
  Serial.println("Conectando");
  
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
}
bool HayIinternet()
{
  Serial.print("Internet");
  //Hago ping 
  if(Ping.ping(url3,3))
  {
    Serial.print("True\n");
    return true;
  }
  Serial.print("False\n");
  return false;
}
void setup() 
{
  Serial.begin(115200);
}

void loop() 
{
  Conectar_Wifi(); 
  do
  {
    if(ConectarAlRouter() < 0)
    {   
      Serial.print("Error al Conectar al Router....Reiniciando Conexion\n");
      break;
    }
    else
    {
      //Leo las cabeceras de la respuesta
      String headerLocation = http.header("Location");
      String headerSet_Cookie = http.header("Set-Cookie");
      if(headerLocation=="http://192.168.1.1/rpSys.html")
      {
        do
        {
          if(SolicitarDatos(headerSet_Cookie) < 0)
          {
            Serial.print("Error al Solicitar Datos....Reiniciando Conexion\n");
            break;
          }
          ProcesarRespuesta();
          delay(3000);       
        }while(true);
      }
      else
      {
        Serial.print("Error al Obtener el Location....Reiniciando Conexion");
        continue;
      }
    }
  }while(true);
}
