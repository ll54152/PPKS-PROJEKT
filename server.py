import socket
import threading
import datetime
import os

# Globalne varijable
client_socket = None
stop_server = False
connected = False

# Funkcija obrade podataka i zapisivanja u datoteku
def handle_client(client_socket, arduino_file):
    global connected
    while not stop_server:
        try:
            data = client_socket.recv(1024).decode('utf-8')
            if not data:
                break
            current_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            # Zapisivanje podataka u datoteku log.txt
            with open(arduino_file, 'a') as f:
                f.write(f"{current_time} - {data}\n")
            connected = True  # Postavljamo da je veza uspostavljena
        except socket.error:
            break
    client_socket.close()


# Funkcija za slanje poruke Arduinu
def send_message(client_socket, message, additional_info):
    client_socket.send(f"{message} - {additional_info}!".encode('utf-8'))

# Glavna funkcija servera
def main():
    global stop_server   # Globalna boolean varijabla za definiranje završetka rada servera
    global connected   # Globalna boolean varijabla za definiranje jeli se neki uređaj spojio na server
    global client_socket

    # Postavljanje IP adrese i porta
    host = '0.0.0.0'
    port = 4253
    arduino_file = 'log.txt'

    # Stvaranje socket objekta
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Povezivanje servera na IP adresu i port
    server_socket.bind((host, port))

    # Slušanje zahtjeva na portu
    server_socket.listen(5)
    print("Server je aktivan i sluša...")

    # Čekanje uređaja
    while not connected:
        client_socket, addr = server_socket.accept()
        print(f"Konekcija od {addr}")

        # Pokretanje zasebne dretve za obradu poruke i zapisivanje u datoteku
        client_handler = threading.Thread(target=handle_client, args=(client_socket, arduino_file))
        client_handler.start()
        connected = True

    while not stop_server:
        try:
            choice = input("Unesite 0 za isključivanje senzora, 1 za uključivanje senzora, 2 za postavljanje Sample Rate-a, 3 za podešavanje pojedinog senzora ili 9 za izlaz: ")

            # Provjera unosa korisnika
            if choice == '0' or choice == '1':
                # Pitamo korisnika za broj senzora
                additional_info = input("Unesite broj senzora od 0 do 9: ")
                send_message(client_socket, f"SensorOff" if choice == '0' else "SensorOn", additional_info)
            elif choice == '2':
                # Pitamo korisnika za dodatni broj milisekundi
                additional_info = input("Unesite broj milisekundi: ")
                send_message(client_socket, "MS", additional_info)
            elif choice == '3':
                # Pitamo korisnika za broj senzora
                sensor_number = input("Unesite broj senzora od 0 do 9: ")
                # Pitamo korisnika za parametre 1 do 5
                param1 = input("Unesite parametar 1: ")
                param2 = input("Unesite parametar 2: ")
                param3 = input("Unesite parametar 3: ")
                param4 = input("Unesite parametar 4: ")
                param5 = input("Unesite parametar 5: ")
                additional_info = f"{sensor_number};{param1};{param2};{param3};{param4};{param5}"
                send_message(client_socket, "SensorConf", additional_info)
            elif choice == '9':
                stop_server = True
                server_socket.close()
                print("Server je isključen.")
                os._exit(1)
                break
            else:
                print("Pogrešan unos!")
        except Exception as e:
            print(f"Greška: {e}")

    # Čekanje da se klijentska nit završi
    if client_socket:
        client_socket.close()

if __name__ == "__main__":
    main()
