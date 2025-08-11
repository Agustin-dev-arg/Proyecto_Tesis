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
}
setInterval(actualizarEstado, 1000);
