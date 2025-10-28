import processing.serial.*;

Serial myPort;           // Port série
String rawData = "";     // ligne brute reçue
float pixsDistance;
int iAngle = 0;
int iDistance = 0;

void setup() {
  size(1366, 768);
  smooth();

  // ⚠️ Change "COM3" to your port, e.g. "COM3" on Windows or "/dev/ttyACM0" on Linux
  myPort = new Serial(this, "COM3", 9600);

  // We read until newline '\n' (typical Arduino println)
  myPort.bufferUntil('\n');
}

void draw() {
  // Background fade for radar effect
  noStroke();
  fill(0, 4);
  rect(0, 0, width, height - height * 0.065);

  // Draw radar and elements
  drawRadar();
  drawLine();
  drawObject();
  drawText();
}

// Called when a full line (ending with '\n') is available
void serialEvent(Serial p) {
  rawData = p.readStringUntil('\n');
  if (rawData == null) return;

  rawData = rawData.trim(); // remove \r, spaces, etc.
  if (rawData.length() == 0) return;

  // Debug: uncomment to see raw incoming lines in Processing console
  // println("RAW: " + rawData);

  // Expecting "angle,distance" like "45,23"
  int comma = rawData.indexOf(',');
  if (comma == -1) {
    // malformed line
    // println("No comma found in: " + rawData);
    return;
  }

  String sAngle = rawData.substring(0, comma).trim();
  String sDistance = rawData.substring(comma + 1).trim();

  // protect parsing
  try {
    iAngle = Integer.parseInt(sAngle);
    iDistance = Integer.parseInt(sDistance);
  } 
  catch (NumberFormatException e) {
    // println("Parse error: " + rawData);
    // keep previous values if parse fails
  }
}

void drawRadar() {
  pushMatrix();
  translate(width / 2, height - height * 0.074); // Centre du radar
  noFill();
  strokeWeight(2);
  stroke(98, 245, 31);

  // Arcs concentriques (cercles du radar)
  arc(0, 0, (width - width * 0.0625), (width - width * 0.0625), PI, TWO_PI);
  arc(0, 0, (width - width * 0.27), (width - width * 0.27), PI, TWO_PI);
  arc(0, 0, (width - width * 0.479), (width - width * 0.479), PI, TWO_PI);
  arc(0, 0, (width - width * 0.687), (width - width * 0.687), PI, TWO_PI);

  // Lignes d’angle
  line(-width / 2, 0, width / 2, 0);
  for (int a = 30; a <= 150; a += 30) {
    line(0, 0, (-width / 2) * cos(radians(a)), (-width / 2) * sin(radians(a)));
  }

  popMatrix();
}

void drawObject() {
  pushMatrix();
  translate(width / 2, height - height * 0.074); // Centre du radar
  strokeWeight(9);
  stroke(255, 10, 10); // Rouge pour l’objet détecté
  // Conversion cm -> pixels (tweak factor if needed)
  pixsDistance = iDistance * ((height - height * 0.1666) * 0.025);

  // Limite la portée à 40 cm
  if (iDistance > 0 && iDistance < 40) {
    // Dessine une ligne rouge représentant l’objet détecté
    line(pixsDistance * cos(radians(iAngle)), -pixsDistance * sin(radians(iAngle)),
         (width - width * 0.505) * cos(radians(iAngle)),
         -(width - width * 0.505) * sin(radians(iAngle)));
  }
  popMatrix();
}

void drawLine() {
  pushMatrix();
  strokeWeight(9);
  stroke(30, 250, 60); // Vert clair pour la ligne de balayage
  translate(width / 2, height - height * 0.074);
  line(0, 0, (height - height * 0.12) * cos(radians(iAngle)),
       -(height - height * 0.12) * sin(radians(iAngle)));
  popMatrix();
}

void drawText() {
  pushMatrix();

  String noObject = (iDistance > 40 || iDistance <= 0) ? "Hors de portée" : "Objet détecté";

  // Fond noir en bas de l’écran
  fill(0, 0, 0);
  noStroke();
  rect(0, height - height * 0.0648, width, height);

  // Texte vert
  fill(98, 245, 31);
  textSize(25);

  // Marques de distances
  text("10cm", width - width * 0.3854, height - height * 0.0833);
  text("20cm", width - width * 0.281, height - height * 0.0833);
  text("30cm", width - width * 0.177, height - height * 0.0833);
  text("40cm", width - width * 0.0729, height - height * 0.0833);

  textSize(40);
  text("Radar Arduino", width - width * 0.875, height - height * 0.0277);
  text("Angle : " + iAngle + "°", width - width * 0.48, height - height * 0.0277);
  text("Distance : ", width - width * 0.26, height - height * 0.0277);

  if (iDistance > 0 && iDistance < 40) {
    text(iDistance + " cm", width - width * 0.18, height - height * 0.0277);
  } else {
    text(noObject, width - width * 0.18, height - height * 0.0277);
  }

  // Marques des angles (30°, 60°, 90°, etc.)
  textSize(25);
  fill(98, 245, 60);
  drawAngleText(30);
  drawAngleText(60);
  drawAngleText(90);
  drawAngleText(120);
  drawAngleText(150);

  popMatrix();
}

// Fonction auxiliaire pour afficher les angles autour du radar
void drawAngleText(int angleValue) {
  pushMatrix();
  translate((width / 2) + (width / 2.1) * cos(radians(angleValue)),
            (height - height * 0.08) - (width / 2.1) * sin(radians(angleValue)));
  rotate(-radians(90 - angleValue));
  text(angleValue + "°", 0, 0);
  popMatrix();
}
