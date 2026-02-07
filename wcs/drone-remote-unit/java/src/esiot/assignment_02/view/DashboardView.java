package esiot.assignment_02.view;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.*;
import esiot.assignment_02.controller.DashboardController;

public class DashboardView extends JFrame implements ActionListener {

    private JButton btnTakeOff;
    private JButton btnLand;
    
    private JTextField txtDroneState;
    private JTextField txtHangarState;
    private JTextField txtDistance;
    private JLabel lblDistance;

    private DashboardController controller; 

    public DashboardView(){
        super(".:: Drone Remote Unit (DRU) ::.");
        setSize(500, 300);
        this.setResizable(false);
        
        JPanel mainPanel = new JPanel();
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
        mainPanel.add(Box.createRigidArea(new Dimension(0,20)));
        
        JPanel pnlDroneState = new JPanel();
        pnlDroneState.setLayout(new BoxLayout(pnlDroneState, BoxLayout.LINE_AXIS));
        txtDroneState = new JTextField("--");
        txtDroneState.setEditable(false);
        txtDroneState.setPreferredSize(new Dimension(150, 20));
        pnlDroneState.add(new JLabel("Drone State:   "));
        pnlDroneState.add(txtDroneState);
        mainPanel.add(pnlDroneState);
        mainPanel.add(Box.createRigidArea(new Dimension(0,10)));

        JPanel pnlHangarState = new JPanel();
        pnlHangarState.setLayout(new BoxLayout(pnlHangarState, BoxLayout.LINE_AXIS));
        txtHangarState = new JTextField("--");
        txtHangarState.setEditable(false);
        txtHangarState.setPreferredSize(new Dimension(150, 20));
        pnlHangarState.add(new JLabel("Hangar State:  "));
        pnlHangarState.add(txtHangarState);
        mainPanel.add(pnlHangarState);
        mainPanel.add(Box.createRigidArea(new Dimension(0,10)));

        //MI SERVE SOLAMENTE DURANTE L'ATTERRAGGIO
        JPanel pnlDistance = new JPanel();
        pnlDistance.setLayout(new BoxLayout(pnlDistance, BoxLayout.LINE_AXIS));
        txtDistance = new JTextField("--");
        txtDistance.setEditable(false);
        txtDistance.setPreferredSize(new Dimension(150, 20));
        lblDistance = new JLabel("Dist. to ground: ");
        pnlDistance.add(lblDistance);
        pnlDistance.add(txtDistance);
        mainPanel.add(pnlDistance);
        
        mainPanel.add(Box.createRigidArea(new Dimension(0,30)));

        JPanel buttonPanel = new JPanel();
        buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));
        
        btnTakeOff = new JButton("Request Take Off");
        btnTakeOff.addActionListener(this);
        
        btnLand = new JButton("Request Landing");
        btnLand.addActionListener(this);
        
        btnTakeOff.setEnabled(false);
        btnLand.setEnabled(false);

        buttonPanel.add(btnTakeOff);
        buttonPanel.add(Box.createRigidArea(new Dimension(20,0))); // Spacer
        buttonPanel.add(btnLand);
        
        mainPanel.add(buttonPanel);
        mainPanel.add(Box.createRigidArea(new Dimension(0,20)));
        
        setContentPane(mainPanel);  
        
        addWindowListener(new WindowAdapter(){
            public void windowClosing(WindowEvent ev){
                System.exit(-1);
            }
        });
    }
    
    public void display() {
        SwingUtilities.invokeLater(() -> {
            this.setVisible(true);
        });
    }

    public void registerController(DashboardController contr){
        this.controller = contr;
    }

	//gestione tramite controller

    public void setDroneState(String state) {
        SwingUtilities.invokeLater(() -> {
            txtDroneState.setText(state);
        });
    }

    public void setHangarState(String state) {
        SwingUtilities.invokeLater(() -> {
            txtHangarState.setText(state);
        });
    }

    public void setDistance(String distance) {
        SwingUtilities.invokeLater(() -> {
            txtDistance.setText(distance);
        });
    }

    public void setControlsEnabled(boolean isTakeOffAllowed, boolean isLandingAllowed) {
        SwingUtilities.invokeLater(() -> {
            btnTakeOff.setEnabled(isTakeOffAllowed);
            btnLand.setEnabled(isLandingAllowed);
        });
    }

    @Override
    public void actionPerformed(ActionEvent ev){
          try {
              if (controller != null) {
                  if (ev.getSource() == btnTakeOff){
                      controller.notifyTakeOff();
                  } else if (ev.getSource() == btnLand){
                      controller.notifyLanding();
                  }
              }
          } catch (Exception ex){
              ex.printStackTrace();
          }
    }
}