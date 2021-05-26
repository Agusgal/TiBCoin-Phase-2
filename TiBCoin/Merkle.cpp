#include"Merkle.h"

string* Merkle::getMerkleTree() {			//entro niveles
	int hojas = 0;
	hojas = ->size();

	hojas = isPar(hojas);				//si es par no lo modifico
	getAltura(hojas);
	IDs(hojas);

}
string* Merkle::IDs(int hojas) {
	treep = new tree[cantidad];			//se crea la lista enlazada
	int aux = 0, auxbis = 0, full = 0, nodos = hojas, indice = 0;
	while (nivel > altura) {
		if (nivel == 1) {
			for (int i = 0; i < hojas; i++) {
				aux = hexcodeAsciitoBin(tx[i].txid);	//leo de la blockchain txid y la paso a binario
				concatenacion(aux);						//concateno el string
				treep[i].id = hexcodeAscii(generateID(buffer));		// genero el id con la concatenacion
				treep[i].nivel = 1;
			}
			indice = hojas;
			nivel += 1;
		}
		else {
			for (int j = 0, int i = full, indice; j < (nodos / 2); indice++, i + 2, j++, full + 2)
			{
				aux = hexcodeAsciitoBin(treep[i].id);	//leo de la blockchain txid y la paso a binario
				auxbis = hexcodeAsciitoBin(treep[i + 1].id);
				concatenacion(aux, auxbis);						//concateno el string
				treep[indice].id = hexcodeAscii(generateID(buffer));		// genero el id con la concatenacion
				treep[indice].nivel = nivel;						//nivel es una variable q me marca el nivel pero del merkle

			}
			nodos = checkNodo(nodos / 2, indice);

			//indice + 2;
			nivel + 1;
		}
	}
}
int Merkle::checkNodo(int nodo, int& indice) {
	if (nodo % 2 == 0) {
		indice + 1;
		return nodo;
	}
	else
	{
		treep[indice + 1] = treep[indice];
		indice + 2;
		return(nodo + 1);
	}
}

void Merkle::getAltura(int hojas) {
	int i = 0, alt = 0, int cant = 0;
	for (i = hojas, cant = hojas; i >= 1; alt++) {
		if (((i / 2) % 2) == 0 || i == 2) {
			i = (i / 2);
			cant += i;
		}
		else {
			i = (i / 2) + 1;
			cant += i;
		}
	}
	cantidad = cant;
	altura = alt;
}

int Merkle::isPar(int hojas) {
	if ((hojas % 2) == 0) {
		return hojas;
	}
	else {
		tx->resize(hojas + 1);			//llevo el nivel a un numero par de hojas
		tx[hojas] = tx[hojas - 1];		//el agregado , es identico a la ultima transaccion
		return hojas + 1;
	}
}



unsigned int Merkle::generateID(unsigned char* str) {
	unsigned int ID = 0;
	int c;
	while (c = *str++) {
		ID = c + (ID << 6) + (ID << 16) - ID;
	}
	return ID;
}
string Merkle::hexcodeAscii(int i) {
	char buffer[255];
	itoa(i, buffer, 16);
	string str;
	int iter = 0;
	for (iter; buffer[iter] == '\0'; iter++) {

		str += buffer[iter];
	}
	return str;
}
int Merkle::hexcodeAsciitoBin(string str) {
	int a = stoi(str, nullptr, 16);
	return(a);
}
unsigned char* Merkle::concatenacion(int ha, int hb = 0) {
	buffer[0] = hb & 0x000000FF;
	buffer[1] = hb & (0x0000FF00) >> 8;
	buffer[2] = hb & (0x00FF0000) >> 16;
	buffer[3] = hb & (0xFF000000) >> 24;
	buffer[4] = ha & 0x000000FF;
	buffer[5] = ha & (0x0000FF00) >> 8;
	buffer[6] = ha & (0x00FF0000) >> 16;
	buffer[7] = ha & (0xFF000000) >> 24;
}