from django.views.decorators.csrf import csrf_exempt
from django.shortcuts import render
from django.http import HttpResponse, JsonResponse

preparacion_activa = False  # Estado inicial de preparación

@csrf_exempt
def actualizar_preparacion(request):
    global preparacion_activa
    if request.method == "POST":
        valor = request.POST.get("preparacion")
    else:
        valor = request.GET.get("preparacion")
    if valor in ["true", "false"]:
        preparacion_activa = (valor == "true")
        return JsonResponse({"status": "ok", "preparacion": preparacion_activa})
    return JsonResponse({"status": "error", "mensaje": "Valor inválido"}, status=400)

def obtener_preparacion(request):
    global preparacion_activa
    return JsonResponse({'preparacion': preparacion_activa})


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