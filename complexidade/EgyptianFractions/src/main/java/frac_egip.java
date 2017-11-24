public class frac_egip {

    private double decimalNumber;
    private double precision;

    public frac_egip(double decimalNumber, double precision){
        this.decimalNumber = decimalNumber;
        this.precision = precision;
    }

    public String calculate(){
        double sum = 0;
        double decimalPrecision = 1/precision;
        StringBuilder sumFractions = new StringBuilder();
        for(int i = 2; i < decimalPrecision; i++){
            double divisor = i;
            sum = sum + 1/divisor;
            if(sum > decimalNumber){
                sum -= 1/divisor;
                continue;
            }
            if(decimalNumber - sum <= precision){
                sumFractions.append("1/").append(i);
                break;
            }else{
                sumFractions.append("1/").append(i).append(" + ");
            }
        }
        return sumFractions.toString();
    }

    public static void main(String args[]){
        frac_egip fracOne = new frac_egip(0.8875, 1e-4);
        System.out.println(fracOne.decimalNumber + " = SUM OF FRACTIONS: " + fracOne.calculate());

        frac_egip fracTwo = new frac_egip(0.8875, 1e-6);
        System.out.println(fracTwo.decimalNumber + " = SUM OF FRACTIONS: " + fracTwo.calculate());
    }
}
