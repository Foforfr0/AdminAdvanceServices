using System;
using System.Threading;

public class CalculaPrimosconHilos
{
    private int limite;

    public CalculaPrimosconHilos(int limite)
    {
        this.limite = limite;
    }

    public void run()
    {
        int contadorPrimos = 0;
        for (int i = 2; i <= limite; i++)
        {
            if (EsPrimo(i))
            {
                contadorPrimos++;
            }
        }
        Console.WriteLine("Número total de primos encontrados: " + contadorPrimos);
    }

    private bool EsPrimo(int numero)
    {
        if (numero <= 1)
        {
            return false;
        }
        for (int i = 2; i <= Math.Sqrt(numero); i++)
        {
            if (numero % i == 0)
            {
                return false;
            }
        }
        return true;
    }

    public static void Main(string[] args)
    {
        int limite = 10000000;
        int maxHilos = 10;
        
        for (int h = 0; h < maxHilos; h++)
        {
            CalculaPrimosconHilos calculador = new CalculaPrimosconHilos(limite);
            Thread hilo = new Thread(new ThreadStart(calculador.run));
            hilo.Start();
        }
    }
}
