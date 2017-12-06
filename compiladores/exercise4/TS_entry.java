import java.util.ArrayList;
/**
 * Write a description of class Paciente here.
 *
 * @author (your name)
 * @version (a version number or a date)
 */
public class TS_entry
{
   private String id;
   private ClasseID classe;
   private TS_entry tipo;


   public TS_entry(String umId, TS_entry umTipo, ClasseID umaClasse) {
      id = umId;
      tipo = umTipo;
      classe = umaClasse;
   }


   public String getId() {
       return id;
   }

   public TS_entry getTipo() {
       return tipo;
   }


   public String toString() {
       StringBuilder aux = new StringBuilder("");

       aux.append("Id: ");
       aux.append(String.format("%-10s", id));

       aux.append("\tClasse: ");
       aux.append(classe);
       aux.append("\tTipo: ");
       aux.append(tipo2str(this.tipo));

      return aux.toString();

   }

  public String getTipoStr() {
       return tipo2str(this);
   }

    public String tipo2str(TS_entry tipo) {
      if (tipo == null)  return "null";
      else if (tipo==Parser.Tp_INT)    return "int";
      else if (tipo==Parser.Tp_BOOLEAN)   return "boolean"; 
      else if (tipo==Parser.Tp_DOUBLE)  return "double";
      else if (tipo==Parser.Tp_ERRO)  return  "_erro_";
      else                             return "erro/tp";
   }

}
