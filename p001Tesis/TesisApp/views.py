from django.views.decorators.csrf import csrf_exempt
from django.shortcuts import render
from django.http import HttpResponse, JsonResponse

@csrf_exempt
def actualizar_estado(request):
    global estado_actual
    if request.method == "POST":
        accion = request.POST.get("accion")
    else:
        accion = request.GET.get("accion")
    if accion in ["start", "stop"]:
        estado_actual = accion
        return JsonResponse({"status": "ok", "estado": estado_actual})
    return JsonResponse({"status": "error", "mensaje": "Acción inválida"}, status=400)

#Definimos una prueba basica para determinar si el ESP32 esta conectado a la red o no.

estado_actual = "stop"  # Estado inicial

def index(request):
    global estado_actual
    if request.method == "POST":
        accion = request.POST.get("accion")
        if accion in ["start", "stop"]:
            estado_actual = accion
    return render(request, "index.html", {"estado": estado_actual})


ultimo_mensaje = "Dispositivo no conectado"
def recibir_mensaje(request):
    global ultimo_mensaje
    mensaje = request.GET.get('mensaje',"Dispositivo no conectado")
    ultimo_mensaje = mensaje
    return JsonResponse({'status': 'Mensaje recibido: ' + mensaje})

def obtener_estado(request):
    global estado_actual
    return JsonResponse({'estado': estado_actual})