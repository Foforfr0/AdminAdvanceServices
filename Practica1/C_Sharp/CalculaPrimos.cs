using System;

public class CalculaPrimos
{
    private int limite;

    public CalculaPrimos(int limite)
    {
        this.limite = limite;
    }

    public void Run()
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
        CalculaPrimos calculador = new CalculaPrimos(limite);
        calculador.Run();
    }
}
