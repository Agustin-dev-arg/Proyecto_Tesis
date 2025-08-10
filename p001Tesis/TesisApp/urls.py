from django.urls import path
from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('recibir_mensaje/', views.recibir_mensaje, name='recibir_mensaje'),  # <-- agrega esta línea
]
