import serial
import time

# Substitua pela sua porta serial
PORTA = '/dev/ttyUSB0'   # ou 'COM3' no Windows
BAUDRATE = 9600

def main():
    try:
        ser = serial.Serial(PORTA, BAUDRATE, timeout=1)
        time.sleep(2)  # Espera o Arduino reiniciar a serial

        print("Digite 1 para enviar $START")
        print("Digite 2 para enviar $RESET")

        while True:
            user_input = input(">> ")

            if user_input == "1":
                ser.write(b"$START\n")
                print("[Python] Enviado: $START")

            elif user_input == "2":
                ser.write(b"$RESET\n")
                print("[Python] Enviado: $RESET")

            else:
                print("Entrada inválida. Use 1 ou 2.")
                continue

            # Lê resposta do Arduino
            resposta = ser.readline().decode().strip()
            if resposta:
                print(f"[Arduino] Respondeu: {resposta}")

    except serial.SerialException as e:
        print(f"Erro ao abrir a porta serial: {e}")
    except KeyboardInterrupt:
        print("\nEncerrando comunicação...")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

if __name__ == "__main__":
    main()
