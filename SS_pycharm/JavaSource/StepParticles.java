import javafx.application.Application;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;

import java.util.Optional;
import java.util.Random;

import javafx.scene.*;
import javafx.scene.paint.*;
import javafx.scene.canvas.*;
import javafx.animation.*;
import javafx.concurrent.Task;


import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javafx.application.Platform;

// Compile with: javac -cp "C:\Program Files\Oracle\JavaFX 2.2 Runtime\lib\jfxrt.jar" StepParticles.java
// Run with:     java -cp "C:\Program Files\Oracle\JavaFX 2.2 Runtime\lib\jfxrt.jar";. StepParticles
// cd "Dropbox\2019\Fall Coursework\OutlineDetectionAndSwarmDynamics\SS_pycharm\JavaSource"
public class StepParticles extends Application {

  private static Particle[] particles;
  //private static Particle[] next; //for multithreadding purposes
  private static Particle[] attractors;
  //private static double[] potentials;
  private static final int xMin = 0;//-200;
  private static final int xMax = 1200;
  private static final int yMin = 0;//-200;
  private static final int yMax = 800;//200;
  private static final double repellentConstant = 10000.0;//5000.0;
  private static final double attractionConstant = 1000.0;


  public static void main(String[] args) {
    launch(args);
  }

  @Override
  public void start(Stage primaryStage) {
      Group root = new Group();
      Scene s = new Scene(root, xMax-xMin, yMax-yMin);
      primaryStage.setScene(s);

      final Canvas canvas = new Canvas(xMax-xMin, yMax-yMin);
      GraphicsContext gc = canvas.getGraphicsContext2D();

      generateAttractors();
      drawAttractors(gc);
      generateParticles();
      drawParticles(gc);
      //gc.fillRect(75,75,100,100);
      int numThreads = 10;
      PotentialThread[] threadPool = new PotentialThread[numThreads];
      System.out.println(particles.length);
      System.out.println(particles.length/numThreads);
      for(int i=0; i<numThreads; i++) {
        if(i!=numThreads-1)
          threadPool[i] = /*new Thread(*/new PotentialThread(i*particles.length/numThreads, (i+1)*particles.length/numThreads);//);
        else
          threadPool[i] = /*new Thread(*/new PotentialThread(i*particles.length/numThreads, particles.length);//);
        //System.out.println("Start: "+i*particles.length/numThreads+" End: "+(i+1)*particles.length/numThreads);
      }

      root.getChildren().add(canvas);
      primaryStage.show();
      double fps = 60.0;
      new AnimationTimer() {
        long lastTime = 0;
            @Override
            public void handle(long now) {
              if(now-lastTime >= 1.0/(fps)*1e9) {
                clear(gc);
                //System.out.println("fps: "+1/((now-lastTime)/1e9));
                drawAttractors(gc);
                drawParticles(gc);
                stepAllParticles();
                /*
                for(PotentialThread t : threadPool) {
                  //t.run();
                  t.call();
                  //t.start();
                }*/
                lastTime = now;
              }
            }
        }.start();
  }

  private static void drawAttractors(GraphicsContext gc) {
    gc.setFill(Color.BLUE);
    for(Particle p : attractors)
      gc.fillOval(p.x, p.y, 2, 2);
  }

  private static void drawParticles(GraphicsContext gc) {
    gc.setFill(Color.RED);
    for(Particle p : particles) {
      if(p.x < 0 || p.y < 0 || p.x > xMax || p.y > yMax)
        System.out.println("particle out of bounds");
      gc.fillOval(p.x, p.y, 5, 5);
    }
  }

  private static void clear(GraphicsContext gc) {
    gc.setFill(Color.WHITE);
    gc.fillRect(0,0,xMax,yMax);
  }

  private static void generateParticles() {
    particles = new Particle[((xMax-xMin)/30+1)*((yMax-yMin)/30+1)];
    int count = 0;
    for(int y = yMin; y <= yMax; y+=30) {
      for(int x = xMin; x <= xMax; x+=30) {
        particles[count] = new Particle(x,y);
        count++;
      }
    }
  }
  private static void generateAttractors() {
    /*
    attractors = new Particle[xMax-xMin+1];
    for(int x = xMin; x <= xMax; x++) {
      attractors[x-xMin] = new Particle(x,x);
    }*/
    attractors = new Particle[yMax+1];
    for(int y = 0; y <= yMax; y++) {
      attractors[y] = new Particle(xMax/2,y);
    }
  }
  private static double potential(int x, int y, int index) {
    double result = 0.0;
    for(int i = 0; i < particles.length; i++) {
      if(i != index) {
        int pX = particles[i].x;
        int pY = particles[i].y;
        double dist = dist(pX,pY,x,y);
        if(dist != 0.0) {
          result += repellentConstant/dist;
        }
        else {
          result += repellentConstant/0.0001;
        }
      }
    }
    for(int i = 0; i < attractors.length; i++) {
      int aX = attractors[i].x;
      int aY = attractors[i].y;
      double dist = dist(aX,aY,x,y);
      if(dist != 0.0) {
        result -= attractionConstant/dist;
      }
      else {
        result -= attractionConstant/0.0001;
      }
    }
    return result;
  }

  // torodial distance
  // Assumes xMin, yMin = 0
  private static double dist(int x1, int y1, int x2, int y2) {
    if(xMin != 0 || yMin != 0)
      throw new AssertionError();
    int dx = Math.abs(x1-x2);
    int dy = Math.abs(y1-y2);
    int trueDx = Math.min(Math.abs(xMax-dx), dx);
    int trueDy = Math.min(Math.abs(yMax-dy), dy);
    return Math.sqrt(trueDx*trueDx + trueDy*trueDy);
  }
  private static Optional<Particle> StepParticle(int index) {
    Random rand = new Random();
    int maxStepSize = 2;
    int xIncr = rand.nextInt(maxStepSize*2+1) - maxStepSize; // either -1, 0, or 1
    int yIncr = rand.nextInt(maxStepSize*2+1) - maxStepSize; // either -1, 0, or 1
    if(xIncr==0 && yIncr==0) {
      return Optional.empty();
    }
    double oldPotential = potential(particles[index].x, particles[index].y, index);
    // enforce torodial bounds
    int newX = mod((particles[index].x + xIncr),xMax);
    int newY = mod((particles[index].y + yIncr),yMax);
    /*
    if(newX < 0) {
      newX = xMax - 5;
    }
    else if(newX > xMax) {
      newX = 0 + 5;
    }
    if(newY < 0) {
      newY = yMax - 5;
    }
    else if(newY > yMax) {
      newY = 0 + 5;
    }*/
    double newPotential = potential(newX, newY, index);
    if(newPotential < oldPotential) {
      return Optional.of(new Particle(newX, newY));
    }
    else {
      if((new java.util.Random().nextDouble())>0.5)
        return Optional.of(new Particle(newX, newY));
    }
    return Optional.empty();
  }
  private static int mod(int d1, int d2) {
    if(d1>=0)
      return d1%d2;
    else
      return d1%d2+d2;
  }
  private static void stepAllParticles() {
    for(int i = 0; i < particles.length; i++) {
      Optional<Particle> newPart = StepParticle(i);
      if(newPart.isPresent()) {
        particles[i] = newPart.get();
      }
    }
  }
  class PotentialThread extends Task<Void>/*extends Thread*/ {
    private int startIndex;
    private int endIndex; //exclusive
    @Override protected Void call() {
      //System.out.println("Thread "+hashCode()+" started");
      for(int i = startIndex; i < endIndex; i++) {
        Optional<Particle> newPart = StepParticle(i);
        if(newPart.isPresent()) {
          particles[i] = newPart.get();
        }
      }
      //System.out.println("Thread "+hashCode()+" done");
      return null;
    }
    /*
    @Override
    public void run() {
      System.out.println("Thread "+getId()+" started");
      for(int i = startIndex; i < endIndex; i++) {
        Optional<Particle> newPart = StepParticle(i);
        if(newPart.isPresent()) {
          particles[i] = newPart.get();
        }
      }
      System.out.println("Thread "+getId()+" done");
    }*/

    // endIndex is exclusive
    public PotentialThread(int startIndex, int endIndex) {
      this.startIndex = startIndex;
      this.endIndex = endIndex;
    }
  }
}

class Particle {
  int x;
  int y;
  public Particle(int x, int y) {
    this.x = x;
    this.y = y;
  }
}
