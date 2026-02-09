plugins {
    // Apply the java plugin to add support for Java
    java

    // Apply the application plugin to add support for building a CLI application
    // You can run your app via task "run": ./gradlew run
    application
    id("com.github.johnrengelman.shadow") version "8.1.1"
}

repositories { // Where to search for dependencies
    mavenCentral()
}

dependencies {
    // Suppressions for SpotBugs
    compileOnly("com.github.spotbugs:spotbugs-annotations:4.8.6")

    implementation("org.eclipse.paho:org.eclipse.paho.client.mqttv3:1.2.5")
    implementation("io.vertx:vertx-mqtt:5.0.7")
    implementation("io.vertx:vertx-core:5.0.7")
    implementation("io.vertx:vertx-web:5.0.7")

    /*
     * Simple Logging Facade for Java (SLF4J) with Apache Log4j
     * See: http://www.slf4j.org/
     */
    val slf4jVersion = "1.7.36"
    implementation("org.slf4j:slf4j-api:$slf4jVersion")
    // Logback backend for SLF4J
    implementation("ch.qos.logback:logback-classic:1.2.12")
    implementation(files("lib/jssc-2.9.4.jar"))
}

application {
    // Define the main class for the application.
    mainClass.set("it.unibo.iot03.main.java")
}