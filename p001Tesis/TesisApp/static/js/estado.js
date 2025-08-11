
function actualizarEstado() {
    fetch('/obtener_estado/')
        .then(response => response.json())
        .then(data => {
            let estado = data.estado;
            let box = document.getElementById('estado-box');
            let text = document.getElementById('estado-text');
            let label = document.getElementById('estado-label');
            if (estado === 'start') {
                box.style.backgroundColor = '#28a745';
            } else {
                box.style.backgroundColor = '#dc3545';
            }
            text.textContent = estado.toUpperCase();
            label.textContent = estado;
        });

    fetch('/obtener_preparacion/')
        .then(response => response.json())
        .then(data => {
            let prep = data.preparacion;
            let prepDiv = document.getElementById('preparacion-indicador');
            if (prep) {
                prepDiv.style.visibility = 'visible';
            } else {
                prepDiv.style.visibility = 'hidden';
            }
        });
}
setInterval(actualizarEstado, 1000);
