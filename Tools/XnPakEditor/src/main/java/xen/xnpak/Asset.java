package xen.xnpak;

public class Asset {
    public String name;
    public AssetType type;
    public String build;

    public Asset(String name, String type, String build) {
        this.name = name;
        this.type = AssetType.valueOf(type);
        this.build = build;
    }
}
