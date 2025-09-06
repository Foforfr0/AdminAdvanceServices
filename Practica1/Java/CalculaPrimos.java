public class CalculaPrimos {
    private int limite;

    public CalculaPrimos(int limite) {
        this.limite = limite;
    }

    public void run() {
        int contadorPrimos = 0;
        for (int i = 2; i <= limite; i++) {
            if (esPrimo(i)) {
                contadorPrimos++;
            }
        }
        System.out.println("Número total de primos encontrados: " + contadorPrimos);
    }

    private boolean esPrimo(int numero) {
        if (numero <= 1) {
            return false;
        }
        for (int i = 2; i <= Math.sqrt(numero); i++) {
            if (numero % i == 0) {
                return false;
            }
        }
        return true;
    }

    public static void main(String[] args) {
        int limite = 10000000;  // Puedes ajustar el límite aquí
        CalculaPrimos calculador = new CalculaPrimos(limite);
        calculador.run();
    }
}
