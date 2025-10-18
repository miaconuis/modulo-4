-- ======================================
-- Inicialización de la tabla 'lecturas'
-- ======================================

-- Crea la tabla si no existe
CREATE TABLE IF NOT EXISTS lecturas (
    id SERIAL PRIMARY KEY,
    dispositivo VARCHAR(50) NOT NULL,
    temperatura FLOAT NOT NULL,
    humedad FLOAT NOT NULL,
    fecha TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Borra cualquier registro previo y reinicia la secuencia del ID
TRUNCATE TABLE lecturas RESTART IDENTITY;

-- Opcional: Ajusta la zona horaria de la sesión a Bogotá (UTC-5)
SET TIMEZONE = 'America/Bogota';

-- Verificación rápida: muestra la estructura de la tabla
\d lecturas;

-- Ejemplo de inserción con hora local (opcional)
-- INSERT INTO lecturas (dispositivo, temperatura, humedad, fecha)
-- VALUES ('maestro1', 25.4, 60.2, NOW());
