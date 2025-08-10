from django.shortcuts import render
from django.http import HttpResponse, JsonResponse

# Create your views here.

#Definimos una prueba basica para determinar si el ESP32 esta conectado a la red o no.

def index(request):
    return HttpResponse("Página de inicio de TesisApp")


ultimo_mensaje = "Dispositivo no conectado"
def recibir_mensaje(request):
    global ultimo_mensaje
    mensaje = request.GET.get('mensaje',"Dispositivo no conectado")
    ultimo_mensaje = mensaje
    return JsonResponse({'status': 'Mensaje recibido: ' + mensaje})