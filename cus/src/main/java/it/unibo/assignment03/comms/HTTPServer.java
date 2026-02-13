package it.unibo.assignment03.comms;

import io.vertx.core.AbstractVerticle;
import io.vertx.core.http.HttpMethod;
import io.vertx.core.http.HttpServerResponse;
import io.vertx.core.json.JsonArray;
import io.vertx.core.json.JsonObject;
import io.vertx.ext.web.Router;
import io.vertx.ext.web.RoutingContext;
import io.vertx.ext.web.handler.BodyHandler;
import io.vertx.ext.web.handler.CorsHandler;
import it.unibo.assignment03.controller.MainController;
import it.unibo.assignment03.model.DataPoint;
import it.unibo.assignment03.model.TankState;

/*
 * Data Service as a vertx event-loop 
 */
public class HTTPServer extends AbstractVerticle {

	private int port;
	private MainController controller;
    private TankState tank;
	
	public HTTPServer(int port, MainController controller,TankState tank) {
		this.controller = controller;	
		this.port = port;
        this.tank = tank;
	}

	@Override
	public void start() {		
		Router router = Router.router(vertx);
		router.route()
			.handler(
				CorsHandler.create()
					.addOrigin("*")
					.allowedMethod(HttpMethod.GET)
					.allowedMethod(HttpMethod.POST)
					.allowedHeader("Content-Type")
				);
		router.route().handler(BodyHandler.create());
		router.get("/api/status").handler(this::handleGetStatus);
		router.get("/api/history").handler(this::handleGetHistory);	
		router.post("/api/mode/switch").handler(this::handleSwitchMode);
        router.post("/api/valve/set").handler(this::handleSetValve);
		vertx
			.createHttpServer()
			.requestHandler(router)
			.listen(port);

		log("Service ready on port: " + port);
	}
	
	private void handleGetStatus(RoutingContext ctx) {
        JsonObject status = new JsonObject();
        status.put("level", tank.getLevel());
        status.put("valve", tank.getValveOpening());
        status.put("mode", tank.getState() == TankState.State.UNCONNECTED ? "UNCONNECTED" : tank.getMode());
        
        ctx.response()
            .putHeader("content-type", "application/json")
            .end(status.encode());
    }
    
    private void handleGetHistory(RoutingContext ctx) {
        JsonArray arr = new JsonArray();
        for (DataPoint p: tank.getHistory()) {
            JsonObject data = new JsonObject();
            data.put("time", p.getTimestamp());
            data.put("value", p.getValue());
            arr.add(data);
        }
        ctx.response()
            .putHeader("content-type", "application/json")
            .end(arr.encode());
    }

    private void handleSwitchMode(RoutingContext ctx) {
        controller.switchMode();
        ctx.response().setStatusCode(200).end();
    }

    private void handleSetValve(RoutingContext ctx) {
        JsonObject body = ctx.body().asJsonObject();
        if (body != null && body.containsKey("value")) {
            int val = body.getInteger("value");
            controller.setValveManual(val);
            ctx.response().setStatusCode(200).end();
        } else {
            ctx.response().setStatusCode(400).end();
        }
    }
	
	private void sendError(int statusCode, HttpServerResponse response) {
		response.setStatusCode(statusCode).end();
	}

	private void log(String msg) {
		System.out.println("[DATA SERVICE] "+msg);
	}

}