from django.urls import path
from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('recibir_mensaje/', views.recibir_mensaje, name='recibir_mensaje'),
    path('obtener_estado/', views.obtener_estado, name='obtener_estado'),
    path('actualizar_estado/', views.actualizar_estado, name='actualizar_estado'),
]
